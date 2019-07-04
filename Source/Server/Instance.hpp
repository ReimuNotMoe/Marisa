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

#ifndef MARISA_SERVER_INSTANCE_HPP
#define MARISA_SERVER_INSTANCE_HPP

#include "../CommonIncludes.hpp"
#include "../App/Route.hpp"
#include "../Log/Log.hpp"

#include "Session.hpp"
#include "../App/App.hpp"

using namespace Marisa::Log;

typedef boost::asio::detail::socket_option::integer<BOOST_ASIO_OS_DEF(SOL_SOCKET), SO_REUSEPORT> so_reuseport;

namespace Marisa {
	namespace Application {
		class Route;
		class AppExposed;
	}

	namespace Server {
		class SessionTCP;
		class SessionSSL;
		class SessionUnix;

		class Request {
		public:
			Request() = default;
		};

		class Response {
		public:
			Response() = default;
		};

		class Instance {
		protected:
			const char *ModuleName = nullptr;
			virtual void prepare_next_session() = 0;
		public:
			boost::asio::io_service io_service;
			boost::asio::io_service::strand io_strand;
			Application::AppExposed &app;
//			std::unique_ptr<ThreadPool> app_thread_pool;

			explicit Instance(Application::AppExposed &__ref_app) : app(__ref_app), io_strand(io_service) {};

			void run();
			void stop();

			virtual void run_impl() = 0;
		};

		class InstanceUnix : public Instance {
		private:
			boost::asio::local::stream_protocol::acceptor acceptor{io_service};

			void prepare_next_session() override;
		public:
			explicit InstanceUnix(Application::AppExposed &__ref_app);

			void listen(const std::string &__path);
			void run_impl() override;
		};


		class InstanceTCP : public Instance {
		protected:
			boost::asio::ip::tcp::acceptor acceptor;
			boost::asio::ip::address addr_listen;

			void prepare_next_session() override;
		public:
			explicit InstanceTCP(Application::AppExposed &__ref_app);

			void listen(const boost::asio::ip::tcp::endpoint& __ep);
			void run_impl() override;
		};

		class InstanceSSL : public InstanceTCP {
		protected:
			boost::asio::ssl::context ssl_ctx;

			void prepare_next_session() override;
		public:
			explicit InstanceSSL(Application::AppExposed &__ref_app);

			void run_impl() override;

			boost::asio::ssl::context& ssl_context() {
				return ssl_ctx;
			}


		};

	}

}

#endif //MARISA_SERVER_INSTANCE_HPP
