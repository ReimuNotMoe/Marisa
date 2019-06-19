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

#ifndef MARISA_APP_MIDDLEWARE_HPP
#define MARISA_APP_MIDDLEWARE_HPP

#include "../CommonIncludes.hpp"
#include "Context.hpp"
#include "../Log/Log.hpp"
#include "Request/Request.hpp"
#include "Response/Response.hpp"

using namespace Marisa::Log;

namespace Marisa {
	namespace Application {
		class ContextExposed;

		class Middleware {
		public:
			ContextExposed *context = nullptr;
			Request::RequestContext *request = nullptr;
			Response::ResponseContext *response = nullptr;

			void reinit_pointers() noexcept;

			void next();

			static std::string encodeURI(const std::string_view& __str) {
				return Protocol::HTTP::Parser::encodeURI(__str);
			}

			static std::string encodeURIComponent(const std::string_view& __str) {
				return Protocol::HTTP::Parser::encodeURIComponent(__str);
			}

			static std::string decodeURI(const std::string_view& __str) {
				return Protocol::HTTP::Parser::decodeURI(__str);
			}

			static std::string decodeURIComponent(const std::string_view& __str) {
				return Protocol::HTTP::Parser::decodeURIComponent(__str);
			}

			virtual void handler();;

			virtual std::unique_ptr<Middleware> New() const;;

			virtual ~Middleware();
		};
	}
}
#endif //MARISA_APP_MIDDLEWARE_HPP
