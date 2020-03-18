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

#ifndef MARISA_UTILITIES_HPP
#define MARISA_UTILITIES_HPP

#include "../Protocol/HTTP/HTTP.hpp"

using namespace Marisa::Protocol::HTTP;

namespace Marisa {
	namespace Utilities {
		template<typename T>
		void explode_string(const std::string &s, char delim, T result) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				*(result++) = item;
			}
		}

		template<typename Task>
		auto post_future_to_strand(boost::asio::io_service::strand& __strand, Task __task) {
			auto package = std::make_shared<std::packaged_task<decltype(__task())()> >(__task);
			auto future  = package->get_future();

			__strand.post([package] { (*package)(); });
			return future;
		}

		extern std::unordered_map<std::string, std::string> mime_types;
		const std::string& get_mime_type(std::string __file_ext);

		std::vector<std::string> explode_string(const std::string &s, char delim);
		std::vector<std::pair<char *, size_t>> explode_zerocopy(const char *haystack, size_t haystack_len, char needle);
		std::vector<std::string_view> explode_zerocopy_sv(const char *haystack, size_t haystack_len, char needle);

		std::string http_status_page(const HTTP::Status &status);
		std::vector<uint8_t> http_status_page_v(const HTTP::Status &status);
		void http_status_page_v(std::vector<uint8_t> &v, const HTTP::Status &status);

		std::string date_string();

		long cpus_available();

	}
}
#endif //MARISA_UTILITIES_HPP
