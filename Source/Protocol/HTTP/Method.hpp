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

#include "CommonIncludes.hpp"

namespace Marisa {
	namespace Protocol {
		namespace HTTP {
			extern const std::unordered_map<std::string, int> Table_Method2Enum;

			class Method {
			private:
				http_method method = (http_method) 0;
			public:
				Method() = default;

				explicit Method(const http_method m) noexcept {
					method = m;
				}

				explicit Method(const int m) noexcept {
					method = (http_method) m;
				}

				explicit Method(const std::string &m) {
					assign(m);
				}

				explicit Method(const char *m) {
					assign(m);
				}

				void assign(const http_method m) {
					method = m;
				}

				void assign(const int m) {
					method = (http_method) m;
				}

				void assign(std::string m) {
					for (auto &it : m) {
						it = (char) toupper(it);
					}

					auto it = Table_Method2Enum.find(m);

					if (it == Table_Method2Enum.end())
						throw std::invalid_argument("not a valid HTTP method :" + m);

					method = (http_method) it->second;
				}

				operator http_method() const noexcept {
					return method;
				}

				int get() const noexcept {
					return method;
				}

				Method &operator=(const std::string &m) {
					assign(m);
					return *this;
				}

				Method &operator=(const int m) {
					assign(m);
					return *this;
				}

				bool operator==(const Method &rhs) const noexcept {
					return method == rhs.method;
				}

				bool operator==(http_method rhs) const noexcept {
					return method == rhs;
				}

				bool operator==(const std::string &rhs) const noexcept {
					return to_string() == rhs;
				}

				bool operator==(const char *rhs) const noexcept {
					return strcmp(to_string(), rhs) == 0;
				}

				const char *to_string() const noexcept {
					return http_method_str(method);
				}
			};

		}
	}
}
