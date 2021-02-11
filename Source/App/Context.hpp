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
//		std::thread app_thread;


//		ssize_t output_buffer_pos = 0;
//		std::vector<uint8_t> output_buffer;
//		std::mutex output_buffer_lock;
//		std::condition_variable output_buffer_clearance;
//
//		void output_buffer_put(const void *buf, size_t len) {
//			std::unique_lock<std::mutex> lg(output_buffer_lock);
//			output_buffer.resize(len);
//			memcpy(output_buffer.data(), buf, len);
//
//			output_buffer_clearance.wait(lg);
//		}
//
//		size_t output_buffer_get(const void *buf, size_t len) {
//			std::unique_lock<std::mutex> lg(output_buffer_lock);
//			output_buffer.resize(len);
//			memcpy(output_buffer.data(), buf, len);
//
//			output_buffer_clearance.wait(lg);
//		}
//
//		void output_buffer_clear() {
//			{
//				std::lock_guard<std::mutex> lg(output_buffer_lock);
//				output_buffer_pos = 0;
//				output_buffer.clear();
//			}
//
//			output_buffer_clearance.notify_one();
//		}

		bool streamed();

		bool streamed_response_done = false;

		void suspend_connection();
		void resume_connection();

		bool match_route();

		void app_container();


		void start_app();
		void wait_app_terminate();

		void process_request();
		void use_default_status_page(int __status);

	public:
		explicit Context(App *__app, struct MHD_Connection *__mhd_conn, const char *__mhd_url, const char *__mhd_method, const char *__mhd_version);

		~Context();

		Request request;
		Response response;

		void next();
	};


}