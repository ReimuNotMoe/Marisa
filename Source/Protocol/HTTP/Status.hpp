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

#ifndef MARISA_PROTOCOL_HTTP_STATUS_HPP
#define MARISA_PROTOCOL_HTTP_STATUS_HPP

#include "CommonIncludes.hpp"

namespace Marisa {
	namespace Protocol {
		namespace HTTP {

			extern const std::unordered_map<std::string, int> Table_Status2Enum;

			class Status {
			private:
				http_status status = (http_status)200;
			public:
				Status() = default;

				explicit Status(const http_status s) noexcept {
					status = s;
				}

				explicit Status(const int s) noexcept {
					status = (http_status) s;
				}

				operator http_status() const noexcept {
					return status;
				}

				Status(const std::string &s) {
					assign(s);
				}

				void assign(const http_status s) {
					status = s;
				}

				void assign(const int s) {
					status = (http_status)s;
				}

				void assign(std::string s) {
					for (auto &it : s) {
						if (isalpha(it))
							it = (char)toupper(it);
						else
							it = '_';
					}

					auto it = Table_Status2Enum.find(s);

					if (it == Table_Status2Enum.end())
						throw std::invalid_argument("not a valid HTTP status :" + s);

					status = (http_status)it->second;
				}

				Status& operator=(const std::string &s) {
					assign(s);
					return *this;
				}

				Status& operator=(const int s) {
					assign(s);
					return *this;
				}

				bool operator==(const Status &rhs) const noexcept {
					return status == rhs.status;
				}

				bool operator==(http_status rhs) const noexcept {
					return status == rhs;
				}

				bool operator==(const std::string &rhs) const noexcept {
					return to_string() == rhs;
				}

				bool operator==(const char *rhs) const noexcept {
					return strcmp(to_string(), rhs) == 0;
				}

				const char *to_string() const noexcept {
					return http_status_str(status);
				}
			};

		}
	}
}

#endif //MARISA_PROTOCOL_HTTP_STATUS_HPP
