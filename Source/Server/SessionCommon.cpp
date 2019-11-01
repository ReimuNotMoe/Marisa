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
#include "../Protocol/HTTP/HTTP1/Generator.hpp"
#include <utility>

using namespace Marisa::Server;
using namespace Marisa::Utilities;
using namespace Marisa::Application;
using namespace Marisa::Log;

Session::Session(Instance& __ref_inst) :
	instance(__ref_inst), app(__ref_inst.app), io_service(__ref_inst.io_service), io_strand(__ref_inst.io_strand), io_timeout_timer(__ref_inst.io_service), io_timeout_duration((long)app.config.connection.timeout_seconds) {
	app_ctx = std::make_unique<Application::ContextExposed>(app, io_service, io_strand);
}

Session::~Session() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tSession teardown\n", ModuleName, (uintptr_t)this);
#endif
}

void Session::inline_async_read() {
	inline_async_read_impl();
}

void Session::inline_async_write(Buffer __data) {
	auto sptr = std::make_shared<Buffer>(std::move(__data));
	io_strand.post(boost::bind(&Session::arrange_inline_async_write, this, my_shared_from_this(), sptr));
}

void Session::arrange_inline_async_write(std::shared_ptr<Session> &keeper, std::shared_ptr<Buffer>& __data) {
	auto &ref = queue_write.emplace_back();
	ref.first = std::move(*__data);

	if (queue_write.size() <= 1) {
		inline_async_write_impl();
	}

}

void Session::reload_context() {
	if (app_ctx->state & Context::FLAG_KEEPALIVE) {
		auto psptr_session = app_ctx->session;
		app_ctx = std::make_unique<Application::ContextExposed>(app, io_service, io_strand);
		app_ctx->session = psptr_session;
		inline_async_read();
	}
}

void Session::decide_io_action_in_read() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdeciding io_action after reading %zu bytes\n", ModuleName, (uintptr_t)this, last_read_size);
#endif

	total_read_size += last_read_size;
	io_timeout_timer.cancel();
	app_ctx->state &= ~Context::STATE_WANT_READ;

	app_ctx->process_request_data(buffer_read.data(), last_read_size);

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdecide_io_action: ssm returned, state=%04" PRIx16 "\n", ModuleName, (uintptr_t)this, app_ctx->state);
#endif

	if (app_ctx->state & Context::STATE_WANT_READ) {
		inline_async_read();
	}
}

void Session::decide_io_action_in_write() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdeciding io_action after write\n", ModuleName, (uintptr_t)this);
#endif

	io_timeout_timer.cancel();

	bool has_something_to_do = false;

	if (app_ctx->state & Context::STATE_THREAD_RUNNING) {
		has_something_to_do = true;
	}

	if (!has_something_to_do && (app_ctx->state & Context::FLAG_KEEPALIVE)) {
		auto psptr_session = app_ctx->session;
		app_ctx = std::make_unique<Application::ContextExposed>(app, io_service, io_strand);
		app_ctx->session = psptr_session;
		inline_async_read();
	}
}

void Session::setup_timeout_timer() {
	io_timeout_timer.expires_after(io_timeout_duration);
	io_timeout_timer.async_wait([this](const boost::system::error_code& error){
		if (error == boost::asio::error::operation_aborted) {
			// DO NOT TOUCH MEMORY HERE
#ifdef DEBUG
			LogD("%s[0x%016" PRIxPTR "]:\tio_timer cancelled\n", "Session???", (uintptr_t)this);
#endif
		} else {
#ifdef DEBUG
			LogD("%s[0x%016" PRIxPTR "]:\tsocket timeout\n", ModuleName, (uintptr_t)this);
#endif
			app_ctx->state = 0;
			close_socket();
		}
	});
}
void Session::setup_timeout_timer_coro() {
	boost::asio::spawn(io_strand, [this](boost::asio::yield_context _yield_ctx) {
			boost::system::error_code ec;
			io_timeout_timer.expires_after(io_timeout_duration);
			io_timeout_timer.async_wait(_yield_ctx[ec]);

			if (ec == boost::asio::error::operation_aborted) {
				// DO NOT TOUCH MEMORY HERE
#ifdef DEBUG
				LogD("%s[0x%016" PRIxPTR "]:\tio_timer cancelled\n", "Session???", (uintptr_t)this);
#endif
			} else {
#ifdef DEBUG
				LogD("%s[0x%016" PRIxPTR "]:\tsocket timeout\n", ModuleName, (uintptr_t)this);
#endif
				app_ctx->state = 0;
				close_socket();
			}
	});
}

std::future<std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>>> Session::read_async(
	size_t __buf_size) {

	return post_future_to_strand(io_strand, boost::bind(&Session::read_async_promised_impl, this, my_shared_from_this(), __buf_size));
}

std::vector<uint8_t> Session::read_async(boost::asio::yield_context& __yield_ctx, size_t __buf_size) {
	return read_async_coro_impl(__yield_ctx, __buf_size);
}

void Session::read_async(std::function<void(const boost::system::error_code &, std::vector<uint8_t>)> __cb,
			 size_t __buf_size) {
	read_async_with_callback_impl(std::move(__cb), __buf_size);
}


std::vector<uint8_t> Session::read_blocking(size_t __buf_size) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_read started\n", ModuleName, (uintptr_t)this);
#endif
	auto f_queue = read_async(__buf_size);
	auto f_async_write = f_queue.get();
	auto result = f_async_write.get();

	if (result.first) {
		throw std::system_error(std::error_code(result.first.value(), result.first.category()), result.first.message());
	}

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_read done\n", ModuleName, (uintptr_t)this);
#endif

	return *result.second;
}

std::future<boost::system::error_code> Session::arrange_async_write(std::shared_ptr<Session> &keeper, std::shared_ptr<Buffer>& __data) {
	auto &ref = queue_write.emplace_back();
	ref.first = std::move(*__data);
	auto f = ref.second.get_future();

	if (queue_write.size() <= 1) {
		write_async_promised_impl();
	}

	return f;
}

std::future<std::future<boost::system::error_code>> Session::write_async(Buffer __data) {
	auto sptr = std::make_shared<Buffer>(std::move(__data));
	return post_future_to_strand(io_strand, boost::bind(&Session::arrange_async_write, this, my_shared_from_this(), sptr));
}

size_t Session::write_async(Buffer __data, boost::asio::yield_context &__yield_ctx) {
	return write_async_coro_impl(std::move(__data), __yield_ctx);
}

void Session::write_async(Buffer __data, std::function<void(const boost::system::error_code &, size_t)> __cb) {
	write_async_with_callback_impl(std::move(__data), std::move(__cb));
}

void Session::write_blocking(Buffer __data) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_write started\n", ModuleName, (uintptr_t)this);
#endif

	auto f_queue = write_async(std::move(__data));
	auto f_async_write = f_queue.get();
	auto errr = f_async_write.get();

	if (errr) {
		throw std::system_error(std::error_code(errr.value(), errr.category()), errr.message());
	}

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_write done\n", ModuleName, (uintptr_t)this);
#endif
}

void Session::error_action(const boost::system::error_code &__err_code) {
	io_timeout_timer.cancel();
	close_socket();
	LogW("%s[0x%016" PRIxPTR "]:\thandler: %s\n", ModuleName, (uintptr_t)this, __err_code.message().c_str());
}

void Session::close_socket() {
	LogD("%s[0x%016" PRIxPTR "]:\trequesting socket to close\n", ModuleName, (uintptr_t)this);
	io_strand.post(boost::bind(&Session::close_socket_impl, this, my_shared_from_this()));
}















