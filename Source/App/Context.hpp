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
	public:
		std::atomic<int> reference_count = 0;

		App *app = nullptr;
		RouteExposed *route = nullptr;

		spdlog::logger *logger = nullptr;

		struct MHD_Connection *mhd_conn = nullptr;
		bool conn_suspended = false;
		std::mutex conn_state_lock;

		size_t processed_post_size = 0;

		bool app_started = false;

		size_t current_middleware = 0;

		std::future<void> app_future;

		/**
		 * Check if current route is streamed.
		 *
		 * @return true for streamed.
		 *
		 */
		bool streamed() const noexcept;

		bool streamed_response_done = false;

		void suspend_connection();
		void resume_connection();

		bool match_route();

		void app_container() noexcept;

		void start_app();
		void wait_app_terminate();

		void process_request();

		/**
		 * Run another middleware in current context.
		 *
		 * Remember to supply the middleware with its constructor. Such as: run(foo("Bar"));
		 *
		 * @param middleware The middleware to run.
		 *
		 */
		void run(Middleware &middleware);

		/**
		 * Run another middleware (lambda style) in current context.
		 *
		 * @param func The middleware to run.
		 *
		 */
		void run(const std::function<void(Request *, Response *, Context *)>& func);

	public:
		explicit Context(App *__app, struct MHD_Connection *__mhd_conn, const char *__mhd_url, const char *__mhd_method, const char *__mhd_version);

		~Context();

		Request request;
		Response response;

	};


}