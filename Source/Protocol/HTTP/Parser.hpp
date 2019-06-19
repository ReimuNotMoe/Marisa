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

#ifndef MARISA_PROTOCOL_HTTP_PARSER_HPP
#define MARISA_PROTOCOL_HTTP_PARSER_HPP

#include "CommonIncludes.hpp"
#include "Status.hpp"
#include "Method.hpp"

namespace Marisa {
	namespace Protocol {
		namespace HTTP {

			class Parser {
			protected:
				std::string __url_raw;
				std::string __url;
				std::unordered_map<std::string, std::string> __headers;
				std::unordered_map<std::string, std::string_view> __headers_lowercase;
				std::unordered_map<std::string, std::string> __query_string;
				std::vector<unsigned char> __body;

				Method __method;
				Status __status;

				size_t __size_parsed = 0;

			public:
				int flags = 0;

				Parser() = default;

				virtual ssize_t parse(const char *data, size_t len) = 0;

//				static size_t encodeURI(const char *s, char *enc);
//				static size_t encodeURIComponent(const char *s, char *enc);
//				static size_t decodeURI(const char *src, char *dst);
				static size_t decodeURIComponent(const char *src, size_t src_size, char *dst);
				static size_t decodeURI(const char *src, size_t src_size, char *dst);

				static std::string encodeURI(const std::string_view& __str);
				static std::string encodeURIComponent(const std::string_view& __str);
				static std::string decodeURI(const std::string_view& __str);
				static std::string decodeURIComponent(const std::string_view &__str);


				const std::unordered_map<std::string, std::string> &headers() const noexcept {
					return __headers;
				}

				const std::unordered_map<std::string, std::string_view> &headers_lowercase() const noexcept {
					return __headers_lowercase;
				}

				const std::unordered_map<std::string, std::string> &query_string() const noexcept {
					return __query_string;
				}

				std::vector<unsigned char> &body() noexcept {
					return __body;
				}

				const std::string &url_raw() const noexcept {
					return __url_raw;
				}

				const std::string &url() const noexcept {
					return __url;
				}

				const Method& method() const noexcept {
					return __method;
				};

				const Status& status() const noexcept {
					return __status;
				};

				size_t size_parsed() const noexcept {
					return __size_parsed;
				}

				virtual bool finished() const noexcept = 0;

				virtual bool headers_finished() const noexcept = 0;
			};
		};
	}
}

#endif //MARISA_PROTOCOL_HTTP_PARSER_HPP
