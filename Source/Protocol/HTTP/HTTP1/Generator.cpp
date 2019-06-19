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
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Generator.hpp"

static const char hdr_part0[] = "HTTP/1.1 ";
static const char hdr_part1[] = " ";
static const char hdr_part2[] = "\r\n";
static const char hdr_part3[] = ": ";
static const char hdr_part4[] = "Keep-Alive: timeout=";
static const char hdr_part5[] = ", max=";
static const char hdr_part6[] = "Connection: Keep-Alive\r\n";

std::string Marisa::Protocol::HTTP1::Generator::generate_headers(ResponseContext& resp_ctx) {
	std::string ret;

	ret.reserve(1024);

	auto sc = std::to_string(resp_ctx.status);
	auto scs = http_status_str(resp_ctx.status);

	ret.append(hdr_part0, hdr_part0 + sizeof(hdr_part0) -1); // HTTP/1.1
	ret.append(sc.c_str(), sc.c_str() + sc.size());
	ret.append(hdr_part1, hdr_part1 + sizeof(hdr_part1) -1);
	ret.append(scs, scs + strlen(scs));
	ret.append(hdr_part2, hdr_part2 + sizeof(hdr_part2) -1);



	// Content-Length
//	if (__headers_lowercase.find("content-length") == __headers_lowercase.end()) {
//		resp_ctx.headers["Content-Length"] = std::to_string(resp_ctx.body.get().size());
//	}


	for (const auto &it : resp_ctx.headers) {
		ret.append(it.first.c_str(), it.first.c_str() + it.first.size());
		ret.append(hdr_part3, hdr_part3 + sizeof(hdr_part3) -1);
		ret.append(it.second.c_str(), it.second.c_str() + it.second.size());
		ret.append(hdr_part2, hdr_part2 + sizeof(hdr_part2) -1);
	}

	// Keep-alive stuff
	if (keepalive_config[0] > 0) {
		std::string buf = std::to_string(keepalive_config[0]);

		// Connection: Keep-Alive
		ret.append(hdr_part6, hdr_part6 + sizeof(hdr_part6) - 1);

		// Keep-Alive: timeout=...
		ret.append(hdr_part4, hdr_part4 + sizeof(hdr_part4) - 1);
		ret.append(buf.c_str(), buf.c_str() + buf.size());
		ret.append(hdr_part5, hdr_part5 + sizeof(hdr_part5) - 1);
		buf = std::to_string(keepalive_config[1]);
		ret.append(buf.c_str(), buf.c_str() + buf.size());
		ret.append(hdr_part2, hdr_part2 + sizeof(hdr_part2) - 1);
	}

	ret.append(hdr_part2, hdr_part2 + sizeof(hdr_part2) - 1);


	return ret;
}

std::string HTTP1::Generator::generate_all(ResponseContext& resp_ctx) {
	// Deprecated, keep for future use
	auto ret = generate_headers(resp_ctx);

//	auto &be = static_cast<const BodyExposed &>(resp_ctx.body);
//
//	const char *s;
//	switch (be.type) {
//		case 1:
//			s = be.str->c_str();
//			ret.append(s, s + be.str->size());
//			break;
//		case 2:
//			ret.append(be.cstr, be.cstr + be.cstr_len);
//			break;
//		default:
//			break;
//	}

	return ret;
}
