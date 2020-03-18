/*
    This file is part of Marisa.
    Copyright (C) 2018-2019 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <vector>
#include <memory>
#include <regex>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "../Server/Instance.hpp"
#include "../Protocol/HTTP/HTTP.hpp"
#include "Middleware.hpp"
#include "Route.hpp"

namespace Marisa {
	namespace Server {
		class InstanceTCP;
		class InstanceSSL;
		class InstanceUnix;
	}

	namespace Application {
		class Route;
		class Middleware_RawIO;

		class App {
		protected:
			std::vector<std::pair<std::regex, std::shared_ptr<Route>>> route_mapping;

			std::shared_ptr<Route> route_global;

			std::vector<std::unique_ptr<Server::InstanceTCP>> tcp_servers;
			std::vector<std::unique_ptr<Server::InstanceSSL>> ssl_servers;
			std::vector<std::unique_ptr<Server::InstanceUnix>> unix_servers;

			std::vector<std::thread> runners;

			void init_seq_routemap();

			// unsigned int app_threads = 1; // No suitable thread pool found so far
			size_t nr_io_threads = 1;

			uint32_t flags = 0;
			std::unique_ptr<Middleware_RawIO> raw_mw = nullptr;

			void run_io(size_t __threads);
		public:
			struct config {
				struct http {
					size_t max_header_size = 10 * 1024;
					uint16_t max_requests_per_conn = 65535;
				} http;

				struct connection {
					uint16_t timeout_seconds = 60;
				} connection;

				struct app {
					bool catch_unhandled_exception = true;
				} app;

				struct global_callbacks {
					std::function<void()> pre_iosvc_run;
				} global_callbacks;

			} config;

			App() = default;

			void listen(uint16_t __port, size_t __instances = 1); // HTTP
			void listen(const std::string &__address, uint16_t __port, size_t __instances = 1);

			void listen_ssl(uint16_t __port, void (*__ssl_setup)(boost::asio::ssl::context &), size_t __instances = 1);
			void listen_ssl(const std::string &__address, uint16_t __port, void (*__ssl_setup)(boost::asio::ssl::context &), size_t __instances = 1);

			void listen_unix(const std::string &__path); // Unix

			Route &route(const std::string &__route);
			Route &route(std::regex __route_regex);

			void use_raw(const Middleware_RawIO& __ref_mw, bool __async=false);

			void run(size_t __threads_per_io_service = 1);
			void stop();

			const std::vector<std::pair<std::regex, std::shared_ptr<Route>>>& get_routes() {
				return route_mapping;
			}

			const std::shared_ptr<Route>& get_route_global() {
				return route_global;
			}
		};

		class AppExposed : public App {
		public:
			using App::route_global;
			using App::route_mapping;

			using App::flags;
			using App::raw_mw;
//			using App::io_threads;
//			using App::app_threads;
		};
	}
}