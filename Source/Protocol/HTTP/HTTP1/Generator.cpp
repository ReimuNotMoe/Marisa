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
#include "../../../Utilities/Utilities.hpp"

static const char hdr_part0[] = "HTTP/1.1 ";
static const char hdr_part1[] = " ";
static const char hdr_part2[] = "\r\n";
static const char hdr_part3[] = ": ";
static const char hdr_part4[] = "Keep-Alive: timeout=";
static const char hdr_part5[] = ", max=";

static const char hdr_part6[] = "Connection: Keep-Alive\r\nKeep-Alive: timeout=";


static std::unordered_map<int, std::string> http1_status_map = {
	{100, "HTTP/1.1 100 Continue\r\n"},
	{101, "HTTP/1.1 101 Switching Protocols\r\n"},
	{102, "HTTP/1.1 102 Processing\r\n"},

	{200, "HTTP/1.1 200 OK\r\n"},
	{201, "HTTP/1.1 201 Created\r\n"},
	{202, "HTTP/1.1 202 Accepted\r\n"},
	{203, "HTTP/1.1 203 Non-Authoritative Information\r\n"},
	{204, "HTTP/1.1 204 No Content\r\n"},
	{205, "HTTP/1.1 205 Reset Content\r\n"},
	{206, "HTTP/1.1 206 Partial Content\r\n"},
	{207, "HTTP/1.1 207 Multi-Status\r\n"},
	{208, "HTTP/1.1 208 Already Reported\r\n"},
	{226, "HTTP/1.1 226 IM Used\r\n"},

	{300, "HTTP/1.1 300 Multiple Choices\r\n"},
	{301, "HTTP/1.1 301 Moved Permanently\r\n"},
	{302, "HTTP/1.1 302 Found\r\n"},
	{303, "HTTP/1.1 303 See Other\r\n"},
	{304, "HTTP/1.1 304 Not Modified\r\n"},
	{305, "HTTP/1.1 305 Use Proxy\r\n"},
	{307, "HTTP/1.1 307 Temporary Redirect\r\n"},
	{308, "HTTP/1.1 308 Permanent Redirect\r\n"},

	{400, "HTTP/1.1 400 Bad Request\r\n"},
	{401, "HTTP/1.1 401 Unauthorized\r\n"},
	{402, "HTTP/1.1 402 Payment Required\r\n"},
	{403, "HTTP/1.1 403 Forbidden\r\n"},
	{404, "HTTP/1.1 404 Not Found\r\n"},
	{405, "HTTP/1.1 405 Method Not Allowed\r\n"},
	{406, "HTTP/1.1 406 Not Acceptable\r\n"},
	{407, "HTTP/1.1 407 Proxy Authentication Required\r\n"},
	{408, "HTTP/1.1 408 Request Timeout\r\n"},
	{409, "HTTP/1.1 409 Conflict\r\n"},
	{410, "HTTP/1.1 410 Gone\r\n"},
	{411, "HTTP/1.1 411 Length Required\r\n"},
	{412, "HTTP/1.1 412 Precondition Failed\r\n"},
	{413, "HTTP/1.1 413 Payload Too Large\r\n"},
	{414, "HTTP/1.1 414 URI Too Long\r\n"},
	{415, "HTTP/1.1 415 Unsupported Media Type\r\n"},
	{416, "HTTP/1.1 416 Range Not Satisfiable\r\n"},
	{417, "HTTP/1.1 417 Expectation Failed\r\n"},
	{421, "HTTP/1.1 421 Misdirected Request\r\n"},
	{422, "HTTP/1.1 422 Unprocessable Entity\r\n"},
	{423, "HTTP/1.1 423 Locked\r\n"},
	{424, "HTTP/1.1 424 Failed Dependency\r\n"},
	{426, "HTTP/1.1 426 Upgrade Required\r\n"},
	{428, "HTTP/1.1 428 Precondition Required\r\n"},
	{429, "HTTP/1.1 429 Too Many Requests\r\n"},
	{431, "HTTP/1.1 431 Request Header Fields Too Large\r\n"},
	{451, "HTTP/1.1 451 Unavailable For Illegal Reasons\r\n"},

	{500, "HTTP/1.1 500 Internal Server Error\r\n"},
	{501, "HTTP/1.1 501 Not Implemented\r\n"},
	{502, "HTTP/1.1 502 Bad Gateway\r\n"},
	{503, "HTTP/1.1 503 Service Unavailable\r\n"},
	{504, "HTTP/1.1 504 Gateway Timeout\r\n"},
	{505, "HTTP/1.1 505 HTTP Version Not Supported\r\n"},
	{506, "HTTP/1.1 506 Variant Also Negotiates\r\n"},
	{507, "HTTP/1.1 507 Insufficient Storage\r\n"},
	{508, "HTTP/1.1 508 Loop Detected\r\n"},
	{510, "HTTP/1.1 510 Not Extended\r\n"},
	{511, "HTTP/1.1 511 Network Authentication Required\r\n"}
};

std::vector<uint8_t> Marisa::Protocol::HTTP1::Generator::generate_headers(ResponseContext& resp_ctx) {
	std::vector<uint8_t> ret(50);

	ret.reserve(1024);

	size_t pos = 0;

	auto sc_ref = http1_status_map.find(resp_ctx.status);

	if (sc_ref != http1_status_map.end()) {
		auto s = sc_ref->second.size();
		memcpy(ret.data(), sc_ref->second.data(), s);
		pos += s;
	}


	for (const auto &it : resp_ctx.headers) {
		auto s_f = it.first.size();
		auto s_s = it.second.size();
		auto s = s_f + s_s + 2 + 2;

		ret.resize(s+pos);
		auto dp = ret.data();

		memcpy(dp+pos, it.first.data(), s_f);
		pos += s_f;

		dp[pos] = ':';
		dp[pos+1] = ' ';
		pos += 2;

		memcpy(dp+pos, it.second.data(), s_s);
		pos += s_s;

		dp[pos] = '\r';
		dp[pos+1] = '\n';
		pos += 2;
	}

	// Keep-alive stuff
	if (keepalive_config[0] > 0) {
		ret.resize(pos+64);
		auto dp = (char *)ret.data();

		memcpy(dp+pos, hdr_part6, sizeof(hdr_part6) - 1);
		pos += sizeof(hdr_part6) - 1;

		auto plen = sprintf(dp+pos, "%u, max=%u\r\n", keepalive_config[0], keepalive_config[1]);
		pos += plen;
	}

	ret.resize(pos+2);
	auto dp = ret.data();
	dp[pos] = '\r';
	dp[pos+1] = '\n';

	return ret;
}

//std::vector<uint8_t> Marisa::Protocol::HTTP1::Generator::generate_headers(ResponseContext& resp_ctx) {
//	std::vector<uint8_t> ret;
//
//	ret.reserve(1024);
//
//		auto sc = std::to_string(resp_ctx.status);
//		auto scs = http_status_str(resp_ctx.status);
//
//		ret.insert(ret.end(), hdr_part0, hdr_part0 + sizeof(hdr_part0) -1); // HTTP/1.1
//		ret.insert(ret.end(), sc.data(), sc.data() + sc.size());
//		ret.insert(ret.end(), hdr_part1, hdr_part1 + sizeof(hdr_part1) -1);
//		ret.insert(ret.end(), scs, scs + strlen(scs));
//		ret.insert(ret.end(), hdr_part2, hdr_part2 + sizeof(hdr_part2) -1);
//	}
//
//
//
//	for (const auto &it : resp_ctx.headers) {
//		ret.insert(ret.end(), it.first.data(), it.first.data() + it.first.size());
//		ret.insert(ret.end(), hdr_part3, hdr_part3 + sizeof(hdr_part3) -1);
//		ret.insert(ret.end(), it.second.data(), it.second.data() + it.second.size());
//		ret.insert(ret.end(), hdr_part2, hdr_part2 + sizeof(hdr_part2) -1);
//	}
//
//	// Keep-alive stuff
//	if (keepalive_config[0] > 0) {
//
//		std::string buf = std::to_string(keepalive_config[0]);
//
//		// Connection: Keep-Alive
//		ret.insert(ret.end(), hdr_part6, hdr_part6 + sizeof(hdr_part6) - 1);
//
//		// Keep-Alive: timeout=...
//		ret.insert(ret.end(), hdr_part4, hdr_part4 + sizeof(hdr_part4) - 1);
//		ret.insert(ret.end(), buf.data(), buf.data() + buf.size());
//		ret.insert(ret.end(), hdr_part5, hdr_part5 + sizeof(hdr_part5) - 1);
//		buf = std::to_string(keepalive_config[1]);
//		ret.insert(ret.end(), buf.data(), buf.data() + buf.size());
//		ret.insert(ret.end(), hdr_part2, hdr_part2 + sizeof(hdr_part2) - 1);
//	}
//
//	ret.insert(ret.end(), hdr_part2, hdr_part2 + sizeof(hdr_part2) - 1);
//
//
//	return ret;
//}

std::vector<uint8_t> HTTP1::Generator::generate_all(ResponseContext& resp_ctx) {
	// Deprecated, keep for future use
	auto ret = generate_headers(resp_ctx);

//	auto &be = static_cast<const BodyExposed &>(resp_ctx.body);
//
//	const char *s;
//	switch (be.type) {
//		case 1:
//			s = be.str->c_str();
//			ret.insert(ret.end(), s, s + be.str->size());
//			break;
//		case 2:
//			ret.insert(ret.end(), be.cstr, be.cstr + be.cstr_len);
//			break;
//		default:
//			break;
//	}

	return ret;
}
