/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../CommonIncludes.hpp"
#include "../Util/Util.hpp"
#include "../Version.hpp"
#include "Context.hpp"
#include "Route.hpp"

using namespace IODash;

namespace Marisa {
	class Route;

	class App {
	public:
		struct Config {
			struct http {
				size_t max_header_size = 10 * 1024;
				size_t max_post_size = 128 * 1024;
				uint16_t max_requests_per_conn = 65535;

			} http;

			struct logging {
				struct internal {
					spdlog::level::level_enum level = spdlog::level::debug;
					std::string pattern = "%Y-%m-%d %T.%f %z [%^%l%$] %v";
					bool stdout_enabled = true;
					std::string file;
				} internal;

				struct access {
					spdlog::level::level_enum level = spdlog::level::debug;
					std::string pattern = "%Y-%m-%d %T.%f %z [%^%l%$] %v";
					bool stdout_enabled = true;
					std::string file;
				} access;
			} logging;

			struct connection {
				uint16_t timeout_seconds = 60;
			} connection;

			struct app {
				bool catch_unhandled_exception = true;
			} app;
		} config;

		std::unique_ptr<spdlog::logger> logger_internal;
		std::unique_ptr<spdlog::logger> logger_access;
	protected:
		int mhd_flags = MHD_ALLOW_SUSPEND_RESUME | MHD_USE_AUTO_INTERNAL_THREAD | MHD_ALLOW_UPGRADE | MHD_USE_ERROR_LOG | MHD_USE_DEBUG;

		int mhd_extra_flags[8] = {MHD_OPTION_END};
		void *mhd_extra_flag_values[8];

		std::string https_cert, https_key, https_trust, https_key_passwd;

		struct MHD_Daemon *mhd_daemon = nullptr;
		SocketAddress<AddressFamily::Any> listen_addr;

		std::unique_ptr<ThreadPool> app_thread_pool_;

		std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> spdlog_internal_sink_stdout;
		std::shared_ptr<spdlog::sinks::daily_file_sink_mt> spdlog_internal_sink_file;

		std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> spdlog_access_sink_stdout;
		std::shared_ptr<spdlog::sinks::daily_file_sink_mt> spdlog_access_sink_file;

		std::vector<std::pair<std::regex, std::shared_ptr<Route>>> route_mapping;
		std::shared_ptr<Route> route_global_;

	protected:
		void init();
		void init_logger();
	protected:
		int mhd_flagslot_unused(int flag = MHD_OPTION_END);

		void mhd_flagslot_set(int flag, void *val);

		static int mhd_connection_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
						  size_t *upload_data_size, void **con_cls);
		static void mhd_request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe);

		static ssize_t mhd_streamed_response_reader(void *cls, uint64_t pos, char *buf, size_t max);
		static void mhd_streamed_response_read_done(void *cls);
	public:
		App();
		App(App::Config cfg);

		static void ignore_sigpipe();

		void listen(uint16_t __port, bool ipv6_enabled = 1);
		void listen_v4(const std::string &__address, uint16_t __port);
		void listen_v6(const std::string &__address, uint16_t __port);

		void set_https_cert(const std::string& str);
		void set_https_cert_file(const std::string& path);
		void set_https_key(const std::string& str);
		void set_https_key_file(const std::string& path);
		void set_https_key_passwd(const std::string& str);
		void set_https_trust(const std::string& str);
		void set_https_trust_file(const std::string& path);

		Route &route(const std::string &__route);
		Route &route(std::regex __route_regex);


		void start(ssize_t io_thread_count = -1);

		void stop();

		std::vector<std::pair<std::regex, std::shared_ptr<Route>>>& routes() noexcept {
			return route_mapping;
		}

		std::shared_ptr<Route>& route_global() noexcept {
			return route_global_;
		}

		ThreadPool& app_thread_pool() noexcept {
			return *app_thread_pool_;
		}
	};
}