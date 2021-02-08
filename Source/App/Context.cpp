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
#include "Middleware.hpp"
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

bool Context::streamed() {
	if (route)
		return route->mode_streamed;
	else
		return false;
}

void Context::process_request() {
	if (route) {
		if (route->middlewares.empty()) {
			logger->error("[{} @ {:x}] Route {}: Method defined but no middlewares", ModuleName, (intptr_t)this, (intptr_t)route);
			use_default_status_page(500);
			return;
		} else {
			return;
		}
	} else { // Route not matched, just send error page on event loop and tear down
		logger->debug("[{} @ {:x}] Route not found", ModuleName, (intptr_t)this, (intptr_t)route);

		use_default_status_page(404);
		return; // Return and teardown
	}
}

void Context::start_app() {
	app_future = app->app_thread_pool().post_work<void>([this]{
		app_container();
	});

	app_started = true;

	logger->debug("[{} @ {:x}] work posted to thread pool", ModuleName, (intptr_t)this);

//	app_thread = std::thread(&Context::app_container, this);
//	app_thread.detach();
}

void Context::wait_app_terminate() {
	if (app_started) {
		app_future.get();
		logger->debug("[{} @ {:x}] app work terminated", ModuleName, (intptr_t) this);
	}
}

void Context::use_default_status_page(int __status) {
	auto &page = Util::default_status_page(__status);
	auto rsp = MHD_create_response_from_buffer(page.size(), (void *)page.c_str(), MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(rsp, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html");
	MHD_queue_response(mhd_conn, __status, rsp);
	MHD_destroy_response(rsp);
}

void Context::app_container() {
	if (app->config.app.catch_unhandled_exception) {
		try {
			next();
		} catch (std::exception &e) {
			logger->error("[{} @ {:x}] uncaught exception in middleware #1 at {:x}: {}", ModuleName, (intptr_t) this, current_middleware,
				      (intptr_t)route->middlewares[current_middleware].get(), e.what());
		}
	} else {
		next();
	}

	logger->debug("[{} @ {:x}] app_container terminated", ModuleName, (intptr_t) this);
}

void Context::next() {
	if (current_middleware > (route->middlewares.size()-1)) {
		// TODO: Empty warning
	} else {
		auto cur_mw = (route->middlewares[current_middleware])->clone();

		cur_mw->__load_context(this);

		logger->debug(R"([{} @ {:x}] calling middleware {} at {:x})", ModuleName, (intptr_t)this, current_middleware, (intptr_t)cur_mw.get());

		current_middleware++;
		cur_mw->handler();
	}
}

bool Context::match_route() {
	// Match URL with routes
	logger->debug(R"([{} @ {:x}] match_route called)", ModuleName, (intptr_t)this);

	std::smatch url_smatch; // Don't let this hell go out of scope
	for (auto &it : app->routes()) {
		if (std::regex_search(request.url(), url_smatch, it.first)) {
			size_t smpos = 0;
			for (auto &its : url_smatch) {
				request.url_matched_capture_groups().push_back(its.str());
				smpos++;
			}
			route = reinterpret_cast<RouteExposed *>(it.second.get());
			logger->debug(R"([{} @ {:x}] route found, ptr={})", ModuleName, (intptr_t)this, (intptr_t)route);

			return true;
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
	{
		std::lock_guard<std::mutex> lg(conn_state_lock);
		MHD_suspend_connection(mhd_conn);
		conn_suspended = true;
	}

	logger->debug(R"([{} @ {:x}] conn suspended)", ModuleName, (intptr_t)this);
	conn_state_cv.notify_one();
}

void Context::resume_connection() {
	std::unique_lock<std::mutex> lg(conn_state_lock);

//	conn_state_cv.wait(lg, [this]{
//		puts("resume_connection: cv wakeup");
//		return conn_suspended;
//	});

	if (!conn_suspended)
		conn_state_cv.wait(lg);

//	while (!MHD_get_connection_info(mhd_conn, MHD_CONNECTION_INFO_CONNECTION_SUSPENDED)->suspended) {
//
//		puts("wait for conn suspension on other end");
//		std::this_thread::yield();
//	}

//	if (MHD_get_connection_info(mhd_conn, MHD_CONNECTION_INFO_CONNECTION_SUSPENDED)->suspended)
//		MHD_resume_connection(mhd_conn);

	if (conn_suspended) {
		MHD_resume_connection(mhd_conn);
		conn_suspended = false;
		logger->debug(R"([{} @ {:x}] conn resumed)", ModuleName, (intptr_t)this);
	} else {
		logger->debug(R"([{} @ {:x}] conn not really resumed)", ModuleName, (intptr_t)this);
	}


}

Context::~Context() {
	wait_app_terminate();
}






