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

#pragma once

#include <vector>
#include <unordered_map>

#include "../../Protocol/HTTP/HTTP.hpp"
#include "../../Types/Date.hpp"
#include "../../Server/ConnectionContext.hpp"

using namespace Marisa::Types;
using namespace Marisa::Protocol;
using namespace Marisa::Server;

namespace Marisa {
	namespace Application {
		class App;
		class AppExposed;
		class Route;
		class Context;

		namespace Request {

			class RequestContext {
			protected:
				HTTP::Parser &http_parser;

				std::string __ip;

				std::unordered_map<std::string, std::string *> ci_lookup_table;

				std::unordered_map<std::string_view, std::string_view> __cookies;

				std::string buf_read;

				Context *context;
			public:
				const HTTP::Method& method;
				const std::unordered_map<std::string, std::string>& headers;
				const std::unordered_map<std::string, std::string_view>& headers_lowercase;
				const std::string& url;
				const std::unordered_map<std::string, std::string>& query;
				const std::unordered_map<std::string_view, std::string_view>& cookies();
				std::vector<std::string> url_smatch;
				const std::string_view body();
				std::vector<uint8_t> read(size_t __buf_size = 4096);

				Socket& socket;

				const std::string& ip();

				RequestContext(Context *__context, HTTP::Parser &__hp, Socket& __s);

			};

			class RequestContextExposed : public RequestContext {
			public:

			};
		}
	}
}