/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Context.hpp"
#include "App.hpp"
#include "Response.hpp"
#include "../Util/Util.hpp"

using namespace Marisa;

const char ModuleName[] = "Context";

Context::Context(App *__app, struct MHD_Connection *__mhd_conn, const char *__mhd_url, const char *__mhd_method, const char *__mhd_version) :
	app(__app), mhd_conn(__mhd_conn), request(this, __mhd_url, __mhd_method, __mhd_version), response(this) {
	logger = app->logger_internal.get();
	match_route();
	static_cast<RequestExposed &>(request).init();
	static_cast<ResponseExposed &>(response).init();
}

bool Context::streamed() const noexcept {
	if (route)
		return route->mode_streamed;
	else
		return false;
}

void Context::process_request() {
	if (route) {
		if (route->middlewares.empty()) {
			logger->error("[{} @ {:x}] Route {:x}: No middlewares", ModuleName, (intptr_t)this, (intptr_t)route);
			response.send_status_page(500);
			return;
		} else {
			return;
		}
	} else { // Route not matched, just send error page on event loop and tear down
		logger->debug("[{} @ {:x}] Route not found", ModuleName, (intptr_t)this, (intptr_t)route);

		response.send_status_page(404);
		return; // Return and teardown
	}
}

void Context::start_app() {
	if (route->mode_async) {
		app_started = true;
		logger->debug("[{} @ {:x}] running app locally", ModuleName, (intptr_t) this);
		app_container();
	} else {
		logger->debug("[{} @ {:x}] running app in thread pool", ModuleName, (intptr_t) this);

		app_future = app->app_thread_pool().enqueue([this] {
			app_container();
		});

		app_started = true;
	}
}

void Context::wait_app_terminate() {
	if (app_started && !route->mode_async) {
		app_future.get();
		logger->debug("[{} @ {:x}] app work terminated", ModuleName, (intptr_t) this);
	}
}

void Context::app_container() noexcept {
	try {
		// Check for previously halted middleware
		if (current_middleware) {
			// Run it again
			logger->debug(R"([{} @ {:x}] middleware processing resumed)", ModuleName, (intptr_t)this);
			current_middleware(&request, &response, this);

			if (process_halted) { // It halted again
				suspend_connection();
				app_started = false;
				logger->debug(R"([{} @ {:x}] middleware processing halted)", ModuleName, (intptr_t)this);
				return;
			} else { // It done processing
				current_middleware_index++;
			}
		}

		for (; current_middleware_index < route->middlewares.size(); current_middleware_index++) {
			current_middleware = route->middlewares[current_middleware_index];

			logger->debug(R"([{} @ {:x}] calling middleware #{})", ModuleName, (intptr_t)this, current_middleware_index);

			current_middleware(&request, &response, this);

			if (process_halted) {
				suspend_connection();
				app_started = false;
				logger->debug(R"([{} @ {:x}] middleware processing halted)", ModuleName, (intptr_t)this);
				return;
			}

			if (static_cast<ResponseExposed &>(response).finalized) {
				logger->debug(R"([{} @ {:x}] response finalized, not advancing middleware index)", ModuleName, (intptr_t)this);
				break;
			}
		}

		if (!static_cast<ResponseExposed &>(response).finalized) {
			logger->warn(R"([{} @ {:x}] response still not finalized after going through all middlewares)", ModuleName, (intptr_t)this);
			static_cast<ResponseExposed &>(response).end();
		}
	} catch (std::exception &e) {
		logger->error("[{} @ {:x}] uncaught exception in middleware #{}: {}", ModuleName, (intptr_t) this, current_middleware_index, e.what());
	}

	logger->debug("[{} @ {:x}] app_container terminated", ModuleName, (intptr_t) this);
}

bool Context::match_route() {
	// Match URL with routes
	logger->debug(R"([{} @ {:x}] match_route called)", ModuleName, (intptr_t)this);

	std::smatch url_smatch; // Don't let this hell go out of scope
	for (auto &it : app->routes()) {
		auto &rte = *std::static_pointer_cast<RouteExposed>(it.second);
		if (rte.path_keys) {
			request.url_vars() = ReGlob::PathMatch(it.first, rte.path_keys.value(), request.url());
			if (!request.url_vars().empty()) {
				route = reinterpret_cast<RouteExposed *>(it.second.get());
				return true;
			}
		} else {
			if (std::regex_search(request.url(), url_smatch, it.first)) {
				size_t smpos = 0;
				for (auto &its : url_smatch) {
					request.url_vars()[std::to_string(smpos)] = std::move(its.str());
//					request.url_matched_capture_groups().push_back(its.str());
					smpos++;
				}
				route = reinterpret_cast<RouteExposed *>(it.second.get());
				logger->debug(R"([{} @ {:x}] route found, ptr={})", ModuleName, (intptr_t) this, (intptr_t) route);

				return true;
			}
		}
	}

	if (app->route_global()) {
		route = static_cast<RouteExposed *>(app->route_global().get());
		logger->debug(R"([{} @ {:x}] global route found, ptr={})", ModuleName, (intptr_t)this, (intptr_t)route);

		return true;
	}

	return false;
}

void Context::suspend_connection() {
	std::unique_lock<std::mutex> lk(conn_state_lock);
	if (!conn_suspended) {
		MHD_suspend_connection(mhd_conn);

		conn_suspended = true;

		logger->debug(R"([{} @ {:x}] conn suspended)", ModuleName, (intptr_t) this);
	} else {
		logger->debug(R"([{} @ {:x}] conn already suspended)", ModuleName, (intptr_t) this);
	}
}

void Context::resume_connection() {
	std::unique_lock<std::mutex> lk(conn_state_lock);

	if (conn_suspended) {
		MHD_resume_connection(mhd_conn);
		conn_suspended = false;

		logger->debug(R"([{} @ {:x}] conn resumed)", ModuleName, (intptr_t)this);
	} else {
		logger->debug(R"([{} @ {:x}] conn already resumed)", ModuleName, (intptr_t)this);
	}

}

void Context::suspend() {
	process_halted = true;
	logger->debug(R"([{} @ {:x}] user halted middleware processing)", ModuleName, (intptr_t)this);
}

void Context::resume() {
	process_halted = false;
	resume_connection();
	logger->debug(R"([{} @ {:x}] user resumed middleware processing)", ModuleName, (intptr_t)this);
}

Context::~Context() {
	wait_app_terminate();
}


