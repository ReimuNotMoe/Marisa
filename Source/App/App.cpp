/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "App.hpp"
#include "Response.hpp"
#include "../Util/Util.hpp"

using namespace Marisa;

const char ModuleName[] = "App";

void App::ignore_sigpipe() {
	signal(SIGPIPE, SIG_IGN);
}

void App::init() {
	init_logger();
	logger_internal->info("Marisa Version " MARISA_VERSION);
}

void App::init_logger() {
	// I know this is stupid, but why spdlog uses a initializer list for sinks?

	auto *env_ll_internal = getenv("MARISA_LOGLEVEL_INTERNAL");
	auto *env_ll_access = getenv("MARISA_LOGLEVEL_ACCESS");

	if (env_ll_internal) {
		config.logging.internal.level = static_cast<spdlog::level::level_enum>(strtol(env_ll_internal, nullptr, 10));
	}

	if (env_ll_access) {
		config.logging.access.level = static_cast<spdlog::level::level_enum>(strtol(env_ll_access, nullptr, 10));
	}

	// Internal
	if (config.logging.internal.stdout_enabled) {
		spdlog_internal_sink_stdout = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		spdlog_internal_sink_stdout->set_level(config.logging.internal.level);
		spdlog_internal_sink_stdout->set_pattern(config.logging.internal.pattern);
	}

	if (!config.logging.internal.file.empty()) {
		spdlog_internal_sink_file = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config.logging.internal.file, 0, 0);
		spdlog_internal_sink_file->set_level(config.logging.internal.level);
		spdlog_internal_sink_file->set_pattern(config.logging.internal.pattern);
	}

	if (spdlog_internal_sink_stdout && !spdlog_internal_sink_file) {
		logger_internal = std::make_unique<spdlog::logger>(spdlog::logger("marisa_internal", {spdlog_internal_sink_stdout}));
	} else if (!spdlog_internal_sink_stdout && spdlog_internal_sink_file) {
		logger_internal = std::make_unique<spdlog::logger>(spdlog::logger("marisa_internal", {spdlog_internal_sink_file}));
	} else if (spdlog_internal_sink_stdout && spdlog_internal_sink_file) {
		logger_internal = std::make_unique<spdlog::logger>(spdlog::logger("marisa_internal", {spdlog_internal_sink_stdout, spdlog_internal_sink_file}));
	} else {
		throw std::logic_error("both log targets can't be disabled at the same time. set log level to off instead.");
	}

	logger_internal->set_level(config.logging.internal.level);

	// Access
	if (config.logging.access.stdout_enabled) {
		spdlog_access_sink_stdout = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		spdlog_access_sink_stdout->set_level(config.logging.access.level);
		spdlog_access_sink_stdout->set_pattern(config.logging.access.pattern);
	}

	if (!config.logging.access.file.empty()) {
		spdlog_access_sink_file = std::make_shared<spdlog::sinks::daily_file_sink_mt>(config.logging.access.file, 0, 0);
		spdlog_access_sink_file->set_level(config.logging.access.level);
		spdlog_access_sink_file->set_pattern(config.logging.access.pattern);
	}

	if (spdlog_access_sink_stdout && !spdlog_access_sink_file) {
		logger_access = std::make_unique<spdlog::logger>(spdlog::logger("marisa_access", {spdlog_access_sink_stdout}));
	} else if (!spdlog_access_sink_stdout && spdlog_access_sink_file) {
		logger_access = std::make_unique<spdlog::logger>(spdlog::logger("marisa_access", {spdlog_access_sink_file}));
	} else if (spdlog_access_sink_stdout && spdlog_access_sink_file) {
		logger_access = std::make_unique<spdlog::logger>(spdlog::logger("marisa_access", {spdlog_access_sink_stdout, spdlog_access_sink_file}));
	} else {
		throw std::logic_error("both log targets can't be disabled at the same time. set log level to off instead.");
	}

	logger_access->set_level(config.logging.access.level);
}

int App::mhd_flagslot_unused(int flag) {
	for (int i=0; i<(sizeof(mhd_extra_flags)/sizeof(int)); i++) {
		if (mhd_extra_flags[i] == MHD_OPTION_END || mhd_extra_flags[i] == flag)
			return i;
	}

	return -1;
}

void App::mhd_flagslot_set(int flag, void *val) {
	int slot = mhd_flagslot_unused(flag);

	if (slot != -1) {
		mhd_extra_flags[slot] = flag;
		mhd_extra_flag_values[slot] = val;
	} else {
		throw std::logic_error("No option slot left");
	}
}

MHD_Result App::mhd_connection_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {

	auto *app = (App *)cls;

	app->logger_internal->debug("[{} @ {:x}] mhd_connection_handler: *con_cls={}", ModuleName, (intptr_t)app, (intptr_t)*con_cls);

	if (!*con_cls) { // First time
		app->logger_internal->debug("[{} @ {:x}] first time", ModuleName, (intptr_t)app);

		*con_cls = new Context(app, connection, url, method, version);
		auto *ctx = (Context *)*con_cls;

		ctx->process_request();
		app->logger_internal->debug("[{} @ {:x}] request processed", ModuleName, (intptr_t)app);
		return MHD_YES;
	} else { // Second or more time
		app->logger_internal->debug("[{} @ {:x}] second or more time", ModuleName, (intptr_t)app);

		auto *ctx = (Context *)*con_cls;

		if (ctx->response.streamed()) {
			if (!ctx->app_started) {
//				ctx->suspend_connection();
				ctx->start_app();
				app->logger_internal->debug("[{} @ {:x}] App started in STREAM mode", ModuleName, (intptr_t)app);
			}

			auto &socket_input = ((RequestExposed *)&ctx->request)->input_sp.first;

			if (*upload_data_size) {
				app->logger_internal->debug("[{} @ {:x}] has post data, size={}", ModuleName, (intptr_t)app, *upload_data_size);


				auto rc_send = socket_input.send(upload_data, *upload_data_size, MSG_NOSIGNAL);

				app->logger_internal->trace("[{} @ {:x}] rc_send={}", ModuleName, (intptr_t)app, rc_send);

				if (rc_send > 0) {
					*upload_data_size -= rc_send;
					if (*upload_data_size) {
						ctx->suspend_connection();
					}
					return MHD_YES;
				} else if (rc_send == 0) {
					app->logger_internal->debug("[{} @ {:x}] rc_send returned 0 (other end close), terminating connection", ModuleName, (intptr_t)app);
					return MHD_NO;
				} else {
					if (errno == EWOULDBLOCK || errno == EAGAIN) {
						app->logger_internal->debug("[{} @ {:x}] rc_send errno is EAGAIN, suspend connection", ModuleName, (intptr_t)app);
						ctx->suspend_connection();
						return MHD_YES;
					} else {
						app->logger_internal->debug("[{} @ {:x}] rc_send errno is {}, terminating connection", ModuleName, (intptr_t)app, errno);
						return MHD_NO;
					}
				}
			} else {
				app->logger_internal->debug("[{} @ {:x}] no post data left", ModuleName, (intptr_t)app);

				if (((RequestExposed *)&ctx->request)->input_sp_send_end_closed) {
					app->logger_internal->debug("[{} @ {:x}] fd_input ALREADY shutdown", ModuleName, (intptr_t)app);
				} else {
					try {
						socket_input.shutdown();
						app->logger_internal->debug("[{} @ {:x}] fd_input shutdown", ModuleName, (intptr_t) app);
					} catch (std::exception &e) {
						app->logger_internal->warn("[{} @ {:x}] shutdown failed: {}", ModuleName, (intptr_t) app, e.what());

					}
//					close(fd_input);
					((RequestExposed *)&ctx->request)->input_sp_send_end_closed = true;
					ctx->conn_state_cv.notify_one();
				}

//				if (ctx->response.stream_started()) {
//					puts("stream started");
				auto *resp = MHD_create_response_from_callback(MHD_SIZE_UNKNOWN, 4096,
									       &App::mhd_streamed_response_reader,
									       ctx,
									       &App::mhd_streamed_response_read_done);
				for (auto &it : ctx->response.header) {
					MHD_add_response_header(resp, it.first.c_str(), it.second.c_str());
				}
				MHD_queue_response(connection, ctx->response.status, resp);
				MHD_destroy_response(resp);
				return MHD_YES;
//				} else {
//					puts("stream not started");
////					ctx->suspend_connection();
//					return MHD_YES;
//				}
			}
		} else { // Not streamed
			if (*upload_data_size) { // Read all post data before starting app
				ctx->processed_post_size += *upload_data_size;
				if (ctx->processed_post_size > app->config.http.max_post_size) {
					app->logger_internal->debug("[{} @ {:x}] post exceeded size, terminating connection", ModuleName, (intptr_t)app);

					return MHD_NO;
				}

				auto &req = static_cast<RequestExposed &>(ctx->request);

				if (!req.mhd_pp) {
					req.mhd_pp = MHD_create_post_processor(connection, 1024, &Marisa::RequestExposed::mhd_post_processor, &req);
				}

				MHD_post_process(req.mhd_pp, upload_data, *upload_data_size);

				*upload_data_size = 0;

				return MHD_YES;
			} else { // No post data left
				if (!ctx->app_started) {
					ctx->suspend_connection();
					ctx->start_app();
					app->logger_internal->debug("[{} @ {:x}] App started in NORMAL mode", ModuleName, (intptr_t)app);

				}
				return MHD_YES;
			}
		}
	}

}

ssize_t App::mhd_streamed_response_reader(void *cls, uint64_t pos, char *buf, size_t max) {
	auto *ctx = (Context *)cls;

	auto &sock = ((ResponseExposed *)&ctx->response)->output_sp.first;

	auto rc_recv = sock.recv(buf, max, 0);

	ctx->app->logger_internal->debug(R"([{} @ {:x}] streamed_response_reader: cls={:x}, fd={}, pos={}, rc_recv={})", ModuleName, (intptr_t)ctx->app, (intptr_t)cls, sock.fd(), pos, rc_recv);

//	printf("streamed_response_reader: cls=%p, fd=%d, rc_recv=%ld\n", cls, fd, rc_recv);

	if (rc_recv > 0) {
		return rc_recv;
	} else if (rc_recv == 0) {
		ctx->app->logger_internal->debug(R"([{} @ {:x}] streamed_response_reader: all data read)", ModuleName, (intptr_t)ctx->app);
		return MHD_CONTENT_READER_END_OF_STREAM;
	} else {
		if (errno == EWOULDBLOCK || errno == EAGAIN) {
			ctx->app->logger_internal->debug(R"([{} @ {:x}] streamed_response_reader: read EAGAIN)", ModuleName, (intptr_t)ctx->app);
			ctx->suspend_connection();
			return 0;
		} else {
			return MHD_CONTENT_READER_END_WITH_ERROR;
		}
	}
}

void App::mhd_streamed_response_read_done(void *cls) {
	auto *ctx = (Context *)cls;
//	auto &sock = ((ResponseContextExposed *)&ctx->response)->output_sp.first;
	try {
		((ResponseExposed *) &ctx->response)->output_sp.first.shutdown();
	} catch (std::exception &e) {
		ctx->app->logger_internal->warn("[{} @ {:x}] streamed_response_read_done: shutdown failed: {}, are you using MacOS??", ModuleName, (intptr_t)ctx->app, e.what());
	}

	ctx->app->logger_internal->debug("[{} @ {:x}] streamed_response_read_done: ctx={}, fd={}", ModuleName, (intptr_t)ctx->app, cls,
					 ((ResponseExposed *)&ctx->response)->output_sp.first.fd());

}

void App::mhd_request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe) {

	auto *app = (App *)cls;
	auto *ctx = (Context *)(*con_cls);

	if (ctx) {
//		auto &fd_input_ref = ((Request::RequestContextExposed *)&ctx->request)->input_sp;
//		auto &fd_output_ref = ((Response::ResponseContextExposed *)&ctx->response)->output_sp;

//		auto fd_input = ((RequestContextExposed *)&ctx->request)->input_sp;
//		auto fd_output = ((ResponseContextExposed *)&ctx->response)->output_sp;

//		fd_input_ref[0] = -1;
		///close(fd_input[0]);
//		fd_output_ref[0] = -1;
		///close(fd_output[0]);
//		fd_input_ref[1] = -1;
//		close(fd_input[1]);
//		fd_output_ref[1] = -1;
//		close(fd_output[1]);
		ctx->conn_state_cv.notify_all();
		delete ctx;
		*con_cls = nullptr;

		app->logger_internal->debug("[{} @ {:x}] request completed", ModuleName, (intptr_t)app);
	}

}

Route &App::route(const std::string &__route) {
	if (__route.empty())
		throw std::invalid_argument("route path can't be an empty string");

	if (__route == "*") {
		route_global_ = std::make_shared<Route>();
		return *route_global_;
	}

//	if (__route[0] != '/')
//		throw std::invalid_argument("non-global route path must begin with a slash '/'");

	auto route_sptr = std::make_shared<Route>();

	std::string route_regex_str;

	if (__route.find(':') != std::string::npos) {
		logger_internal->debug(R"([{} @ {:x}] route contains variables)", ModuleName, (intptr_t)this);

		auto p = ReGlob::PathResolve(__route);
		route_regex_str = ReGlob::RegexpString(p.first, {.capture = true}, true);
		std::static_pointer_cast<RouteExposed>(route_sptr)->path_keys = std::move(p.second);
	} else {
		route_regex_str = ReGlob::RegexpString(__route, {
			.bash_syntax = true,
			.full_match = true,
			.capture = true,
		});
	}

	std::regex route_regex(route_regex_str, std::regex::ECMAScript | std::regex::optimize);

	logger_internal->debug(R"([{} @ {:x}] adding route "{}": regex="{}", ptr={:x})", ModuleName, (intptr_t)this, __route, route_regex_str, (uintptr_t)route_sptr.get());

	route_mapping.emplace_back(std::move(route_regex), route_sptr);

	return *route_sptr;
}

Route &App::route(std::regex __route_regexp) {
	auto route_sptr = std::make_shared<Route>();
	route_mapping.emplace_back(std::move(__route_regexp), route_sptr);

	return *route_sptr;
}

void App::listen(uint16_t __port, bool ipv6_enabled) {
	if (ipv6_enabled) {
		listen_addr.as_ipv6()->from_string("::");
		listen_addr.as_ipv6()->port() = __port;
		listen_addr.family() = AddressFamily::IPv6;

		mhd_flags |= MHD_USE_DUAL_STACK;
	} else {
		listen_addr.as_ipv4()->from_string("0.0.0.0");
		listen_addr.as_ipv4()->port() = __port;
		listen_addr.family() = AddressFamily::IPv4;

		mhd_flags &= ~MHD_USE_DUAL_STACK;
	}
}

void App::listen_v4(const std::string &__address, uint16_t __port) {
	listen_addr.as_ipv4()->from_string(__address);
	listen_addr.as_ipv4()->port() = __port;
	listen_addr.family() = AddressFamily::IPv4;
}

void App::listen_v6(const std::string &__address, uint16_t __port) {
	listen_addr.as_ipv6()->from_string(__address);
	listen_addr.as_ipv6()->port() = __port;
	listen_addr.family() = AddressFamily::IPv6;
}

void App::set_https_cert(const std::string &str) {
	https_cert = str;
	mhd_flags |= MHD_USE_TLS;
	mhd_flagslot_set(MHD_OPTION_HTTPS_MEM_CERT, (void *)https_cert.c_str());
}

void App::set_https_cert_file(const std::string &path) {
	set_https_cert(Util::read_file(path));
}

void App::set_https_key(const std::string &str) {
	https_key = str;
	mhd_flags |= MHD_USE_TLS;
	mhd_flagslot_set(MHD_OPTION_HTTPS_MEM_KEY, (void *)https_key.c_str());
}

void App::set_https_key_file(const std::string &path) {
	set_https_key(Util::read_file(path));
}

void App::set_https_key_passwd(const std::string &str) {
	https_key_passwd = str;
	mhd_flagslot_set(MHD_OPTION_HTTPS_KEY_PASSWORD, (void *)https_key_passwd.c_str());
}

void App::set_https_trust(const std::string &str) {
	https_trust = str;
	mhd_flagslot_set(MHD_OPTION_HTTPS_MEM_TRUST, (void *)https_trust.c_str());
}

void App::set_https_trust_file(const std::string &path) {
	set_https_trust(Util::read_file(path));
}

App::App() {
	init();
}

App::App(App::Config cfg) {
	config = std::move(cfg);
	init();
}

void App::start(ssize_t io_thread_count) {
	if (listen_addr.family() != AddressFamily::IPv4 && listen_addr.family() != AddressFamily::IPv6) {
		throw std::invalid_argument("Please set a listening address before starting server");
	}

	if (io_thread_count == -1) {
		io_thread_count = std::thread::hardware_concurrency();
		logger_internal->info("Your machine's hardware concurrency is {}", io_thread_count);
	}

	ignore_sigpipe();

	logger_internal->info("Starting server on {} with {} I/O threads ...", listen_addr.to_string(), io_thread_count);
	logger_internal->debug("[{} @ {:x}] start: mhd_flags=0x{:x}", ModuleName, (intptr_t)this, mhd_flags);

	app_thread_pool_ = std::make_unique<ThreadPool>(32);

	mhd_daemon = MHD_start_daemon(mhd_flags, 0,
				      nullptr, nullptr,
				      &mhd_connection_handler, this,
				      MHD_OPTION_NOTIFY_COMPLETED, &mhd_request_completed, this,
				      MHD_OPTION_SOCK_ADDR, listen_addr.raw(),
				      MHD_OPTION_THREAD_POOL_SIZE, io_thread_count,
//				      MHD_OPTION_CONNECTION_MEMORY_LIMIT, 256 * 1024,
				      mhd_extra_flags[0], mhd_extra_flag_values[0],
				      mhd_extra_flags[1], mhd_extra_flag_values[1],
				      mhd_extra_flags[2], mhd_extra_flag_values[2],
				      mhd_extra_flags[3], mhd_extra_flag_values[3],
				      mhd_extra_flags[4], mhd_extra_flag_values[4],
				      mhd_extra_flags[5], mhd_extra_flag_values[5],
				      mhd_extra_flags[6], mhd_extra_flag_values[6],
				      mhd_extra_flags[7], mhd_extra_flag_values[7],
				      MHD_OPTION_END);

	logger_internal->info("Server successfully started");

	logger_internal->debug("[{} @ {:x}] start: mhd_daemon=0x{:x}", ModuleName, (intptr_t)this, (intptr_t)mhd_daemon);

	if (!mhd_daemon) {
		throw std::logic_error("Failed to start server");
	}
}

void App::stop() {
	logger_internal->info("Stopping server on {} ...", listen_addr.to_string());

	if (mhd_daemon) {
		MHD_stop_daemon(mhd_daemon);
		mhd_daemon = nullptr;
		logger_internal->info("Server successfully stopped");
	} else {
		logger_internal->info("Server already stopped");
	}
}








