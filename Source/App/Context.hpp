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

#include "App.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "../Util/Util.hpp"

namespace Marisa {

	class App;

	class Route;
	class RouteExposed;
	class Middleware;

	class Response;

	class Request;
	class RequestExposed;

	class Context {
	protected:
		// Global contexts
		App *app = nullptr;
		RouteExposed *route = nullptr;
		struct MHD_Connection *mhd_conn = nullptr;

		// Connection states
		bool conn_suspended = false;
		std::mutex conn_state_lock;
		size_t processed_post_size = 0;
		bool streamed_response_done = false;

		// Middleware execution states
		bool app_started = false;
		bool process_halted = false;
		size_t current_middleware_index = 0;
		std::function<void(Request *, Response *, Context *)> current_middleware;
		std::future<void> app_future;

	protected:
		void suspend_connection();
		void resume_connection();

		bool match_route();

		void app_container() noexcept;

		void start_app();
		void wait_app_terminate();

		void process_request();

	public:
		/**
		 * Check if current route is streamed.
		 *
		 * @return true for streamed.
		 *
		 */
		bool streamed() const noexcept;

		/**
		 * Halt the processing of middleware chain.
		 *
		 * Has no effect on currently running middleware.
		 * When resumed, current middleware will be executed again.
		 *
		 *
		 */
		void suspend();

		/**
		 * Resume the processing of middleware chain.
		 *
		 * Usually called from another thread.
		 * When resumed, last executed middleware will be executed again.
		 *
		 *
		 */
		void resume();

	public:
		explicit Context(App *__app, struct MHD_Connection *__mhd_conn, const char *__mhd_url, const char *__mhd_method, const char *__mhd_version);

		~Context();

		Request request;
		Response response;

		spdlog::logger *logger = nullptr;
	};

	class ContextExposed : public Context {
	public:
		using Context::app;
		using Context::route;
		using Context::logger;
		using Context::mhd_conn;

		using Context::conn_suspended;
		using Context::conn_state_lock;
		using Context::processed_post_size;
		using Context::streamed_response_done;

		using Context::app_started;
		using Context::process_halted;
		using Context::current_middleware_index;
		using Context::current_middleware;
		using Context::app_future;

		using Context::suspend_connection;
		using Context::resume_connection;
		using Context::match_route;
		using Context::app_container;
		using Context::start_app;
		using Context::wait_app_terminate;
		using Context::process_request;
	};


}