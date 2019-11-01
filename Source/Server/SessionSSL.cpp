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

#include "Session.hpp"
#include "../App/App.hpp"
#include "../App/Context.hpp"
#include "../Log/Log.hpp"
#include "../Utilities/Utilities.hpp"


using namespace Marisa::Server;
using namespace Marisa::Utilities;
using namespace Marisa::Application;
using namespace Marisa::Log;

SessionSSL::SessionSSL(Instance& __ref_inst, boost::asio::ssl::context &ssl_ctx, size_t io_buf_size) :
	Session::Session(__ref_inst), ssl_socket(io_service, ssl_ctx) {

	buffer_read.resize(io_buf_size);
}

void SessionSSL::start() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tStarted\n", ModuleName, (uintptr_t)this);
#endif

	conn_ctx = std::make_unique<SocketTCP>(static_cast<boost::asio::ip::tcp::socket &>(ssl_socket.lowest_layer()));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tNew connection from %s:%" PRIu16 "\n", ModuleName, (uintptr_t)this,
	     conn_ctx->remote_endpoint().address().to_string().c_str(), conn_ctx->remote_endpoint().port());
#endif

	app_ctx->session = this;

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tStarting async handshake\n", ModuleName, (uintptr_t)this);
#endif

	ssl_socket.async_handshake(boost::asio::ssl::stream_base::server,
				   boost::bind(&SessionSSL::handler_handshake, this, shared_from_this(),
					       boost::asio::placeholders::error));

}

void SessionSSL::handler_handshake(std::shared_ptr<SessionSSL>& __session_keeper, const boost::system::error_code &error) {
	if (error) {
		io_timeout_timer.cancel();
		close_socket();
		LogW("%s[0x%016" PRIxPTR "]:\thandler_handshake: %s\n", ModuleName, (uintptr_t) this, error.message().c_str());
		return;
	}

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tHandshake done\n", ModuleName, (uintptr_t)this);
#endif

	if (app.raw_mw) {
		app_ctx->run_raw_handler();
	} else {
		inline_async_read_impl();
	}
}

void SessionSSL::inline_async_read_impl() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tinline_async_read: begin\n", ModuleName, (uintptr_t)this);
#endif

	setup_timeout_timer();

	ssl_socket.async_read_some(boost::asio::buffer(buffer_read.data(), buffer_read.size()),
				   [this, s = shared_from_this()](const boost::system::error_code &error, size_t bytes_transferred){
					   if (error) {
						   error_action(error);
						   return;
					   }

#ifdef DEBUG
					   LogD("%s[0x%016" PRIxPTR "]:\tinline_async_read_impl: read %zu bytes\n", ModuleName, (uintptr_t) this, bytes_transferred);
#endif
					   last_read_size = bytes_transferred;

					   decide_io_action_in_read();
				   });

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tinline_async_read_impl: queued\n", ModuleName, (uintptr_t)this);
#endif
}

void SessionSSL::inline_async_write_impl() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tinline_async_write_impl: begin\n", ModuleName, (uintptr_t)this);
#endif
	const auto& current_package = queue_write.front();
	auto& current_data = current_package.first;

	setup_timeout_timer();

	boost::asio::async_write(ssl_socket, current_data.get(),
				 io_strand.wrap([this, s = shared_from_this()](const boost::system::error_code &error, size_t size_written) {
#ifdef DEBUG
					 LogD("%s[0x%016" PRIxPTR "]:\tinline_async_write_impl: handler called\n", ModuleName, (uintptr_t)this);
#endif
					 auto& current_package = queue_write.front();
					 current_package.second.set_value(error);
					 queue_write.pop_front();

					 if (error) {
						 error_action(error);
						 return;
					 }

					 if (!queue_write.empty())
						 write_async_promised_impl();
				 }));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tinline_async_write_impl: queued, size=%zu\n", ModuleName, (uintptr_t)this, current_data.size());
#endif
}

std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>> SessionSSL::read_async_promised_impl(
	std::shared_ptr<Session> &__session_keeper, size_t __buf_size) {
	auto data = std::make_shared<std::vector<uint8_t>>(__buf_size);
	auto promise = std::make_shared<std::promise<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>>>();

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tread_async_promised_impl: begin\n", ModuleName, (uintptr_t)this);
#endif

	setup_timeout_timer();

	boost::asio::async_read(ssl_socket, boost::asio::buffer(data->data(), data->size()),
				io_strand.wrap([this, s = shared_from_this(), data, promise](const boost::system::error_code &error, size_t bytes_transferred) {
#ifdef DEBUG
					LogD("%s[0x%016" PRIxPTR "]:\tread_async_promised_impl: handler called\n", ModuleName, (uintptr_t)this);
#endif
					data->resize(bytes_transferred);
					promise->set_value(std::make_pair(error, data));
					if (error) {
						error_action(error);
						return;
					}


				}));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tread_async_promised_impl: queued\n", ModuleName, (uintptr_t)this);
#endif

	return promise->get_future();
}


std::vector<uint8_t> SessionSSL::read_async_coro_impl(boost::asio::yield_context &__yield_ctx, size_t __buf_size) {
	std::vector<uint8_t> ret(__buf_size);
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tread_async_coro_impl: begin\n", ModuleName, (uintptr_t)this);
#endif
	setup_timeout_timer();

	auto rc = ssl_socket.async_read_some(boost::asio::buffer(ret.data(), ret.size()), __yield_ctx);
	ret.resize(rc);
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tread_async_coro_impl: done\n", ModuleName, (uintptr_t)this);
#endif
	return ret;
}

void SessionSSL::read_async_with_callback_impl(
	std::function<void(const boost::system::error_code &, std::vector<uint8_t>)> __cb, size_t __buf_size) {
	auto data = std::make_shared<std::vector<uint8_t>>(__buf_size);

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tread_async_with_callback_impl: begin\n", ModuleName, (uintptr_t)this);
#endif

	setup_timeout_timer();

	ssl_socket.async_read_some(boost::asio::buffer(data->data(), data->size()),
				   io_strand.wrap([this, s = shared_from_this(), data, cb = std::move(__cb)](const boost::system::error_code &error, size_t bytes_transferred){
#ifdef DEBUG
					   LogD("%s[0x%016" PRIxPTR "]:\tread_async_with_callback_impl: handler called\n", ModuleName, (uintptr_t)this);
#endif
					   data->resize(bytes_transferred);

					   if (error) {
						   error_action(error);
					   }

					   cb(error, std::move(*data));
				   }));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tread_async_with_callback_impl: queued, size=%zu\n", ModuleName, (uintptr_t)this, data->size());
#endif
}

size_t SessionSSL::write_async_coro_impl(Buffer __data, boost::asio::yield_context &__yield_ctx) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite_async_coro_impl: begin\n", ModuleName, (uintptr_t)this);
#endif
	setup_timeout_timer_coro();

	auto s = boost::asio::async_write(ssl_socket, __data.get(), __yield_ctx);
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite_async_coro_impl: done\n", ModuleName, (uintptr_t)this);
#endif
	return s;
}

void SessionSSL::write_async_promised_impl() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite_async_promised_impl: begin\n", ModuleName, (uintptr_t)this);
#endif
	const auto& current_package = queue_write.front();
	auto& current_data = current_package.first;

	setup_timeout_timer();

	boost::asio::async_write(ssl_socket, current_data.get(),
				 io_strand.wrap([this, s = shared_from_this()](const boost::system::error_code &error, size_t size_written) {
#ifdef DEBUG
					 LogD("%s[0x%016" PRIxPTR "]:\twrite_async_promised_impl: handler called\n", ModuleName, (uintptr_t)this);
#endif
					 auto& current_package = queue_write.front();
					 current_package.second.set_value(error);
					 queue_write.pop_front();

					 if (error) {
						 error_action(error);
						 return;
					 }

					 if (!queue_write.empty())
						 write_async_promised_impl();
				 }));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite_async_promised_impl: queued, size=%zu\n", ModuleName, (uintptr_t)this, current_data.size());
#endif
}

void SessionSSL::write_async_with_callback_impl(Buffer __data,
						std::function<void(const boost::system::error_code &, size_t)> __cb) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite_async_with_callback_impl: begin\n", ModuleName, (uintptr_t)this);
#endif
	setup_timeout_timer();

	auto buf = std::make_shared<Buffer>(std::move(__data));
	boost::asio::async_write(ssl_socket, buf->get(), io_strand.wrap(
		[this, s = shared_from_this(), sb = buf, cb = std::move(__cb)]
			(const boost::system::error_code &error, size_t size_written) {
#ifdef DEBUG
			LogD("%s[0x%016" PRIxPTR "]:\twrite_async_with_callback_impl: handler called\n", ModuleName, (uintptr_t)this);
#endif

			if (error) {
				error_action(error);
			}

			cb(error, size_written);

		}
	));
}

void SessionSSL::close_socket_impl(std::shared_ptr<Session>& keeper) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tclose_socket_impl: called\n", ModuleName, (uintptr_t)this);
#endif
	boost::system::error_code ec;
	socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	if (ec)
		LogE("%s[0x%016" PRIxPTR "]:\tclose_socket_impl: %s\n", ModuleName, (uintptr_t)this, ec.message().c_str());
}

std::shared_ptr<Session> SessionSSL::my_shared_from_this() {
	return std::static_pointer_cast<Session>(shared_from_this());
}







