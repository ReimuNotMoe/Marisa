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

#ifndef MARISA_APP_RESPONSE_BODY_HPP
#define MARISA_APP_RESPONSE_BODY_HPP

#include "../../CommonIncludes.hpp"

namespace Marisa {
	namespace Application {
		class App;
		class AppExposed;
		class Route;

		namespace Response {
			class ResponseContext;
			class ResponseContextExposed;

			class Body {
			protected:
				ResponseContext &resp_ctx;

				std::unique_ptr<std::string> str;

				const char *cstr = nullptr;
				size_t cstr_len = 0;

				int type = 0;

				void set_content_type(const char *__type);
			public:
				explicit Body(ResponseContext &__ref_resp) : resp_ctx(__ref_resp) {};

				Body& operator=(const std::string& rhs); // Depends, mime = text/html
				Body& operator=(const char *rhs); // Copies data, mime = text/html

				void assign_string(const std::string &s); // Depends, mime = text/html
				void assign_cstring(const char *s); // Copies data, mime = text/html
				void assign_cstring(const char *s, size_t l); // Copies data, mime = text/html
				void assign_data(const char *s, size_t l); // Copies data, mime = application/octet-stream

				void emplace_string(std::string &s); // Moves data, mime = text/html
				void emplace_cstring(const char *s); // Stores pointer, mime = text/html
				void emplace_cstring(const char *s, size_t l); // Stores pointer, mime = text/html
				void emplace_data(const char *s, size_t l); // Stores pointer, mime = application/octet-stream

				std::string_view get();

			};

			class BodyExposed : public Body {
			public:
				using Body::type;
				using Body::str;
				using Body::cstr;
				using Body::cstr_len;

				explicit BodyExposed(ResponseContext &__ref_resp) : Body(__ref_resp) {};
			};
		}
	}
}
#endif //MARISA_APP_RESPONSE_BODY_HPP
