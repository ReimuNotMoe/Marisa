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

	inline_async_read_impl();
}

std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>> SessionSSL::read_promised_impl(
	std::shared_ptr<Session> &__session_keeper, size_t __buf_size) {
	auto data = std::make_shared<std::vector<uint8_t>>(__buf_size);
	auto promise = std::make_shared<std::promise<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>>>();

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tasync_read: begin\n", ModuleName, (uintptr_t)this);
#endif

	setup_timeout_timer();

	boost::asio::async_read(ssl_socket, boost::asio::buffer(data->data(), data->size()),
				io_strand.wrap([this, s = shared_from_this(), data, promise](const boost::system::error_code &error, size_t bytes_transferred){
					data->resize(bytes_transferred);
					promise->set_value(std::make_pair(error, data));
					if (error) {
						error_action(error);
						return;
					}

#ifdef DEBUG
					LogD("%s[0x%016" PRIxPTR "]:\thandler_read called\n", ModuleName, (uintptr_t)this);
#endif
				}));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tasync_read: queued, size=%zu\n", ModuleName, (uintptr_t)this, __data->size());
#endif

	return promise->get_future();
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
					   LogD("%s[0x%016" PRIxPTR "]:\thandler_inline_read: read %zu bytes\n", ModuleName, (uintptr_t) this, bytes_transferred);
#endif

					   last_read_size = bytes_transferred;

					   decide_io_action_in_read();
	});
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tinline_async_read: Done\n", ModuleName, (uintptr_t)this);
#endif
}

size_t SessionSSL::write_async_impl(Buffer __data, boost::asio::yield_context &__yield_ctx) {
	return boost::asio::async_write(ssl_socket, __data.get(), __yield_ctx);
}

void SessionSSL::write_promised_impl() {
	const auto& current_package = queue_write.front();
	auto& current_data = current_package.first;

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tasync_write: begin\n", ModuleName, (uintptr_t)this);
#endif

	setup_timeout_timer();

	boost::asio::async_write(ssl_socket, current_data.get(),
				 io_strand.wrap([this, s = shared_from_this()](const boost::system::error_code &error, size_t size_written){
					 auto& current_package = queue_write.front();
					 current_package.second.set_value(error);
					 queue_write.pop_front();

					 if (error) {
						 error_action(error);
						 return;
					 }

#ifdef DEBUG
					 LogD("%s[0x%016" PRIxPTR "]:\thandler_write called\n", ModuleName, (uintptr_t)this);
#endif

					 if (!queue_write.empty())
						 write_promised_impl();
					 else
						 decide_io_action_in_write();
				 }));

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tasync_write: queued, size=%zu\n", ModuleName, (uintptr_t)this, current_data.size());
#endif
}

void SessionSSL::close_socket_impl(std::shared_ptr<Session>& keeper) {
	try {
		socket().close();
	} catch (...) {

	}
}

std::shared_ptr<Session> SessionSSL::my_shared_from_this() {
	return std::static_pointer_cast<Session>(shared_from_this());
}





