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
	instance(__ref_inst), app(__ref_inst.app), io_service(__ref_inst.io_svc), io_strand(__ref_inst.io_svc), io_timeout_timer(__ref_inst.io_svc), io_timeout_duration((long)app.config.connection.timeout_seconds) {
	app_ctx = std::make_unique<Application::ContextExposed>(app);
}

Session::~Session() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tSession teardown\n", ModuleName, (uintptr_t)this);
#endif
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
		app_ctx = std::make_unique<Application::ContextExposed>(app);
		app_ctx->session = psptr_session;
		inline_async_read();
	}
}

void Session::setup_timeout_timer() {
	io_timeout_timer.expires_from_now(io_timeout_duration);
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

std::future<std::future<std::pair<boost::system::error_code, std::shared_ptr<std::vector<uint8_t>>>>> Session::async_read(size_t __buf_size) {

	auto s = my_shared_from_this();
	return post_future_to_strand(io_strand, [&](){
		return async_read_impl(s, __buf_size);
	});

//	return post_future_to_strand(io_strand, boost::bind(&Session::async_read_impl, this, my_shared_from_this(), __buf_size));
}

void Session::inline_async_read() {
	inline_async_read_impl();
}

std::vector<uint8_t> Session::blocking_read(size_t __buf_size) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_read started\n", ModuleName, (uintptr_t)this);
#endif
	auto f_queue = async_read(__buf_size);
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

std::future<boost::system::error_code> Session::arrange_async_write(std::shared_ptr<Session> &keeper, std::shared_ptr<std::string>& __data) {
	auto &ref = queue_write.emplace_back();
	ref.first = std::move(*__data);
	auto f = ref.second.get_future();

	if (queue_write.size() <= 1) {
		async_write_impl();
	}

	return f;
}

std::future<std::future<boost::system::error_code>> Session::async_write(std::string __data) {
	auto sptr = std::make_shared<std::string>(std::move(__data));
	return post_future_to_strand(io_strand, boost::bind(&Session::arrange_async_write, this, my_shared_from_this(), sptr));
}


void Session::blocking_write(std::string __data) {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_write started\n", ModuleName, (uintptr_t)this);
#endif

	auto f_queue = async_write(std::move(__data));
	auto f_async_write = f_queue.get();
	auto errr = f_async_write.get();

	if (errr) {
		throw std::system_error(std::error_code(errr.value(), errr.category()), errr.message());
	}

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tblocking_write done\n", ModuleName, (uintptr_t)this);
#endif
}

void Session::handler_timeout(const boost::system::error_code& error) {
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
}

void Session::error_action(const boost::system::error_code &__err_code) {
	io_timeout_timer.cancel();
	close_socket();
	LogW("%s[0x%016" PRIxPTR "]:\thandler: %s\n", ModuleName, (uintptr_t)this, __err_code.message().c_str());
}

void Session::close_socket() {
	io_strand.post(boost::bind(&Session::close_socket_impl, this, my_shared_from_this()));
}





