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

#ifndef MARISA_SERVER_SESSION_HPP
#define MARISA_SERVER_SESSION_HPP

#include "../CommonIncludes.hpp"
#include "../App/Context.hpp"
#include "../App/Route.hpp"
#include "../App/App.hpp"
#include "../Log/Log.hpp"
#include "ConnectionContext.hpp"
#include "Instance.hpp"

using namespace Marisa::Log;
using namespace Marisa::Application;


namespace Marisa {
	namespace Application {
		class Route;
		class App;
		class AppExposed;
		class Context;
		class ContextExposed;
	}

	namespace Server {
		class Instance;

		class Session {
		protected:
			std::vector<uint8_t> buffer_read;
			size_t last_read_size = 0;

			AppExposed& app;

			std::unique_ptr<Application::ContextExposed> app_ctx;
			boost::asio::io_service &io_service;
			boost::asio::io_service::strand io_strand;
			boost::asio::deadline_timer io_timeout_timer;
			boost::posix_time::seconds io_timeout_duration;

			std::deque<std::pair<std::string, std::promise<boost::system::error_code>>> queue_write;

			virtual void decide_io_action_in_read();
			virtual void decide_io_action_in_write();

			virtual void setup_timeout_timer();
			virtual void error_action(const boost::system::error_code& __err_code);

		public:
			size_t total_read_size = 0;

			std::unique_ptr<Socket> conn_ctx;
			Instance& instance;

			explicit Session(Instance& __ref_inst);

			virtual ~Session();

			const char *ModuleName = nullptr;

			virtual void start() = 0;



			virtual std::future<std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>>> async_read(size_t __buf_size = 4096);
			virtual void inline_async_read();
			virtual std::vector<uint8_t> blocking_read(size_t __buf_size = 4096);


			virtual void inline_async_read_impl() = 0;
			virtual std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>> async_read_impl(std::shared_ptr<Session>& __session_keeper, size_t __buf_size) = 0;


			virtual std::future<boost::system::error_code> arrange_async_write(std::shared_ptr<Session>& keeper, std::shared_ptr<std::string>& __data);
			virtual std::future<std::future<boost::system::error_code>> async_write(std::string __data);
			virtual void blocking_write(std::string __data);

			virtual void async_write_impl() = 0;



			virtual void close_socket();
			virtual void close_socket_impl(std::shared_ptr<Session>& keeper) = 0;

			void handler_timeout(const boost::system::error_code& error);

			virtual int fd() = 0;

			virtual std::shared_ptr<Session> my_shared_from_this() = 0;
		};


		class SessionTCP : public Session, public std::enable_shared_from_this<SessionTCP> {
		private:
			boost::asio::ip::tcp::socket tcp_socket;
		protected:


		public:
			explicit SessionTCP(Instance& __ref_inst, size_t io_buf_size = 1536);


			boost::asio::ip::tcp::socket &socket() {
				return tcp_socket;
			}

			int fd() override {
				return socket().native_handle();
			}

			void start() override;


			std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>> async_read_impl(std::shared_ptr<Session>& __session_keeper, size_t __buf_size) override;

			void inline_async_read_impl() override;


			void async_write_impl() override;

			void close_socket_impl(std::shared_ptr<Session>& keeper) override;

			std::shared_ptr<Session> my_shared_from_this() override;
		};

		class SessionSSL : public Session, public std::enable_shared_from_this<SessionSSL> {
		private:
			boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
		public:

			SessionSSL(Instance& __ref_inst, boost::asio::ssl::context &ssl_ctx, size_t io_buf_size = 1536);

			boost::asio::ip::tcp::socket &socket() {
				return static_cast<boost::asio::ip::tcp::socket &>(ssl_socket.lowest_layer());
			}

			int fd() override {
				return socket().native_handle();
			}

			void start() override;


			std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>> async_read_impl(std::shared_ptr<Session>& __session_keeper, size_t __buf_size) override;

			void inline_async_read_impl() override;

			void async_write_impl() override;

			void close_socket_impl(std::shared_ptr<Session>& keeper) override;

			void handler_handshake(std::shared_ptr<SessionSSL>& __session_keeper, const boost::system::error_code &error);


			std::shared_ptr<Session> my_shared_from_this() override;
		};

		class SessionUnix : public Session, public std::enable_shared_from_this<SessionUnix> {
		private:
			boost::asio::local::stream_protocol::socket unix_socket;
		protected:

		public:

			explicit SessionUnix(Instance& __ref_inst, size_t io_buf_size = 1536);


			boost::asio::local::stream_protocol::socket &socket() {
				return unix_socket;
			}

			int fd() override {
				return socket().native_handle();
			}

			void start() override;

			std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>> async_read_impl(std::shared_ptr<Session>& __session_keeper, size_t __buf_size) override;

			void inline_async_read_impl() override;

			void async_write_impl() override;

			void close_socket_impl(std::shared_ptr<Session>& keeper) override;


			std::shared_ptr<Session> my_shared_from_this() override;
		};
	}
}

#endif //MARISA_SERVER_SESSION_HPP
