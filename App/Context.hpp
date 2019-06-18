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

#ifndef MARISA_APP_CONTEXT_HPP
#define MARISA_APP_CONTEXT_HPP

#include "../CommonIncludes.hpp"
#include "App.hpp"
#include "Response/Response.hpp"
#include "Request/Request.hpp"
#include "../Protocol/HTTP/HTTP.hpp"
#include "../Protocol/HTTP/HTTP1/HTTP1.hpp"

using namespace Marisa::Protocol;

namespace Marisa {
	namespace Server {
		class Session;
	}

	namespace Application {
		class App;
		class AppExposed;
		class Route;
		class RouteExposed;
		class Middleware;

		namespace Response {
			class ResponseContext;
		}

		namespace Request {
			class RequestContext;
		}

		class Context {
		public:
			enum {
				// IO states: 0x0000 - 0x00ff
				STATE_WANT_READ = 0x1, STATE_WANT_WRITE = 0x2,
				STATE_IO_ERROR = 0x4, STATE_THREAD_RUNNING = 0x8,

				// Flags: 0x0100 - 0xff00
				FLAG_NOT_STREAMED = 0x0100, FLAG_ASYNC = 0x0200, FLAG_KEEPALIVE = 0x0400,
				FLAG_COMP_GZIP = 0x1000, FLAG_COMP_DEFLATE = 0x2000
			};

			struct HandlerData {
				size_t pos_cur_handler = 0;
				std::vector<std::unique_ptr<Middleware>> &middleware_list;

				explicit HandlerData(std::vector<std::unique_ptr<Middleware>> &__ref_mw_list) : middleware_list(__ref_mw_list) {};
			};

		protected:
			Application::AppExposed &app;
			Server::Session *session = nullptr;

			std::unique_ptr<HTTP::Parser> http_parser;
			std::unique_ptr<HTTP::Generator> http_generator;

			uint16_t state = 0;
			std::shared_ptr<RouteExposed> route;
			std::unique_ptr<HandlerData> handlers;

			std::thread container;

			static void container_thread(Context *__ctx, void *__session_sptr);

			bool init_handler_data();
			bool determine_hp_state();
			void process_request_data(uint8_t *__buf, size_t __len);
			void use_default_status_page(const HTTP::Status &__status);

		public:
			explicit Context(Application::AppExposed &__ref_app);

			std::unique_ptr<Request::RequestContext> request;
			std::unique_ptr<Response::ResponseContext> response;

			void next();
		};

		class ContextExposed : public Context {
		public:
			using Context::http_parser;
			using Context::http_generator;

			using Context::session;

			using Context::route;
			using Context::handlers;
			using Context::state;

			using Context::container_thread;
			using Context::process_request_data;

			explicit ContextExposed(AppExposed &__ref_app) : Context(__ref_app) {};

		};
	}

}

#endif //MARISA_APP_CONTEXT_HPP
