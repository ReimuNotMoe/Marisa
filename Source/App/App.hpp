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
			struct logging {
				struct internal {
					spdlog::level::level_enum level = spdlog::level::debug;		/*!< Loglevel of internal components */
					std::string pattern = "%Y-%m-%d %T.%f %z [%^%l%$] %v";		/*!< Log pattern of internal components. See spdlog manual for details */
					bool stdout_enabled = true;					/*!< Log to standard output */
					std::string file;						/*!< File path to log to. Disabled if unset */
				} internal;

				struct access {
					spdlog::level::level_enum level = spdlog::level::debug;		/*!< Loglevel of access logs */
					std::string pattern = "%Y-%m-%d %T.%f %z [%^%l%$] %v";		/*!< Log pattern of access logs. See spdlog manual for details */
					bool stdout_enabled = true;					/*!< Log to standard output */
					std::string file;						/*!< File path to log to. Disabled if unset */
				} access;
			} logging;

			struct connection {
				uint16_t timeout_seconds = 60;						/*!< Connection timeout in seconds */
				size_t max_connections = 10485760;					/*!< Max allowed connections */
				size_t max_post_size = 128 * 1024;					/*!< Max size of POST data allowed in normal mode */
			} connection;

			struct app {
				size_t thread_pool_size_ratio = 6;					/*!< Use x threads for every processor core */
			} app;

			bool ignore_sigpipe = true;							/*!< Ignores SIGPIPE */
		} config;

		std::unique_ptr<spdlog::logger> logger_internal;
		std::unique_ptr<spdlog::logger> logger_access;
	protected:
		int mhd_flags = MHD_ALLOW_SUSPEND_RESUME | MHD_USE_AUTO_INTERNAL_THREAD | MHD_ALLOW_UPGRADE | MHD_USE_ERROR_LOG | MHD_USE_DEBUG;

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
		static MHD_Result mhd_connection_handler(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
						  size_t *upload_data_size, void **con_cls);
		static void mhd_request_completed(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe);

		static ssize_t mhd_streamed_response_reader(void *cls, uint64_t pos, char *buf, size_t max);
		static void mhd_streamed_response_read_done(void *cls);

		static void ignore_sigpipe();
	public:
		App();
		App(App::Config cfg);

		/**
		 * Listen on specified port.
		 *
		 * @param port The port to listen on.
		 * @param ipv6_enabled Enable IPv6 (dual stack).
		 *
		 */
		void listen(uint16_t port, bool ipv6_enabled = 1);

		/**
		 * Listen on specified IPv4 address.
		 *
		 * @param address Address in string form. e.g. "0.0.0.0"
		 * @param port Port. e.g. 8080
		 *
		 */
		void listen_v4(const std::string &address, uint16_t port);

		/**
		 * Listen on specified IPv6 address.
		 *
		 * @param address Address in string form. e.g. "::"
		 * @param port Port. e.g. 8080
		 *
		 */
		void listen_v6(const std::string &address, uint16_t port);

		/**
		 * Set HTTPS certificate (string form).
		 *
		 * @param str PEM certificate in string form.
		 *
		 */
		void set_https_cert(const std::string& str);

		/**
		 * Set HTTPS certificate (file).
		 *
		 * @param path Path to a PEM certificate.
		 *
		 */
		void set_https_cert_file(const std::string& path);

		/**
		 * Set HTTPS private key (string form).
		 *
		 * @param str Private key in string form.
		 *
		 */
		void set_https_key(const std::string& str);

		/**
		 * Set HTTPS private key (file).
		 *
		 * @param path Path to a private key.
		 *
		 */
		void set_https_key_file(const std::string& path);

		/**
		 * Set password of the HTTPS private key.
		 *
		 * @param str Password of the HTTPS private key.
		 *
		 */
		void set_https_key_passwd(const std::string& str);


		void set_https_trust(const std::string& str);
		void set_https_trust_file(const std::string& path);

		/**
		 * Add a route denoted by a glob or path expression.
		 *
		 * e.g. "/file/*", "/user/:id".
		 * You can access the variables from request->url_vars() .
		 * For the global route, use a single asterisk ("*").
		 *
		 * @param expression Route expression.
		 * \return Created route object.
		 *
		 */
		Route &route(const std::string &expression);

		/**
		 * Add a route denoted by a regular expression object.
		 *
		 * You can access the captured strings from requests->url_vars() .
		 *
		 * @param regexp Regular expression object.
		 * \return Created route object.
		 *
		 */
		Route &route(std::regex regexp);

		/**
		 * Start the server.
		 *
		 * @param io_thread_count Threads used for network I/O handling. -1 for auto detect.
		 *
		 */
		void start(ssize_t io_thread_count = -1);

		/**
		 * Stop the server.
		 *
		 */
		void stop();

		/**
		 * Registered routes on this App.
		 *
		 */
		std::vector<std::pair<std::regex, std::shared_ptr<Route>>>& routes() noexcept {
			return route_mapping;
		}

		/**
		 * Global route on this App.
		 *
		 */
		std::shared_ptr<Route>& route_global() noexcept {
			return route_global_;
		}

		/**
		 * Thread pool of this App.
		 *
		 */
		ThreadPool& app_thread_pool() noexcept {
			return *app_thread_pool_;
		}
	};
}