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

#include <iostream>
#include "Request.hpp"
#include "../Context.hpp"
#include "../../Utilities/Utilities.hpp"

using namespace Marisa::Application::Request;
using namespace Marisa::Utilities;

Marisa::Application::Request::RequestContext::RequestContext(Marisa::Application::Context *__context,
							     HTTP::Parser &__hp, Socket &__s) : context(__context), http_parser(__hp), method(__hp.method()), headers(__hp.headers()), headers_lowercase(__hp.headers_lowercase()), url(__hp.url()), query(__hp.query_string()), socket(__s) {

}

const std::string_view RequestContext::body() {
	auto &b = http_parser.body();
	return {(const char *)b.data(), b.size()};
}

std::vector<uint8_t> Marisa::Application::Request::RequestContext::read(size_t __buf_size) {
	if (!http_parser.body().empty()) {
		auto r = std::move(http_parser.body());
		http_parser.body().clear();
		return r;
	}

	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	return sess->blocking_read(__buf_size);
}

const std::unordered_map<std::string_view, std::string_view> &RequestContext::cookies() {
	if (__cookies.empty()) {
		auto it = headers_lowercase.find("cookie");

		if (it != headers_lowercase.end()) {
			auto &cstr = it->second;

			// Explode by ';'
			auto single_cookie = explode_zerocopy_sv(cstr.data(), cstr.size(), ';');

			for (auto &it_c : single_cookie) {

				std::cout << "sc: " << it_c << "|\n";

				// Explode by '='
				auto equal_mark_pos = it_c.find('=');

				if (equal_mark_pos == it_c.npos) {
					__cookies[it_c] = std::string_view();
				} else {
					size_t key_offset = 0;
					auto key_len = equal_mark_pos;

					while (it_c[key_len-1] == ' ' /*&& key_len > 0*/)
						key_len--;


					while (it_c[key_offset] == ' ' && key_offset < it_c.size()-1) {
						key_offset++;
						key_len--;
					}

					auto key = std::string_view(it_c.data()+key_offset, key_len);

					size_t val_offset = equal_mark_pos+1;
					auto val_len = it_c.size()-equal_mark_pos-1;

					while (it_c[val_offset] == ' ') {
						val_offset++;
						val_len--;
					}

					while (it_c[val_offset+val_len-1] == ' ')
						val_len--;

					auto val = std::string_view(it_c.data()+val_offset, val_len);

					__cookies[key] = val;
				}
			}
		}
	}

	return __cookies;
}

const std::string &RequestContext::ip() {
	if (__ip.empty()) {

		auto it = headers_lowercase.find("x-forwarded-for");

		if (it != headers_lowercase.end()) {
			__ip = it->second;
			auto cpos = __ip.find(',');
			if (cpos != __ip.npos) {
				__ip.resize(cpos);
			}
		} else {
			__ip = socket.remote_endpoint().address().to_string();
		}
	}

	return __ip;
}
