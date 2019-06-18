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

#include "Parser.hpp"
#include "../../../Utilities/Utilities.hpp"

using namespace Marisa::Utilities;
using namespace Marisa::Protocol::HTTP1;

int HTTP1::Parser::callback_url(http_parser *hp_ctx, const char *at, size_t length) {
	auto ctx = (Parser *) hp_ctx->data;

//	if (ctx->Debug)
//		debug("callback_url", hp_ctx, at, length);

	ctx->__url_raw.append(at, at + length);
	return 0;
}

int HTTP1::Parser::callback_body(http_parser *hp_ctx, const char *at, size_t length) {
	auto ctx = (Parser *) hp_ctx->data;

//	if (ctx->Debug)
//		debug("callback_body", hp_ctx, at, length);

	ctx->__body.insert(ctx->__body.end(), at, at + length);
	return 0;
}

int HTTP1::Parser::callback_status(http_parser *hp_ctx, const char *at, size_t length) {
	auto ctx = (Parser *) hp_ctx->data;

//	if (ctx->Debug)
//		debug("callback_status", hp_ctx, at, length);

	ctx->__status.assign(ctx->hp_ctx.status_code);

	return 0;
}

int HTTP1::Parser::callback_header_field(http_parser *hp_ctx, const char *at, size_t length) {
	auto ctx = (Parser *) hp_ctx->data;

//	if (ctx->Debug)
//		debug("callback_header_field", hp_ctx, at, length);

	if (ctx->last_data_cb == callback_header_field) {
		ctx->last_field.append(at, at + length);
	} else {
		if (!ctx->last_field.empty()) {
			ctx->__headers.insert({std::move(ctx->last_field), std::move(ctx->last_value)});
//			ctx->__headers[std::move(ctx->last_field)] = std::move(ctx->last_value);
		}

		ctx->last_field.assign(at, at + length);
	}

	ctx->last_data_cb = callback_header_field;
	return 0;
}

int HTTP1::Parser::callback_header_value(http_parser *hp_ctx, const char *at, size_t length) {
	auto ctx = (Parser *) hp_ctx->data;
//
//	if (ctx->Debug)
//		debug("callback_header_value", hp_ctx, at, length);

	if (ctx->last_data_cb == callback_header_value) {
		ctx->last_value.append(at, at + length);
	} else {
		ctx->last_value.clear();
		ctx->last_value.assign(at, at + length);
	}

	ctx->last_data_cb = callback_header_value;
	return 0;
}

int HTTP1::Parser::callback_message_complete(http_parser *hp_ctx) {
	auto ctx = (Parser *) hp_ctx->data;

	ctx->__finished = true;

	return 0;
}

int HTTP1::Parser::callback_headers_complete(http_parser *hp_ctx) {
	auto ctx = (Parser *) hp_ctx->data;

	if (!ctx->last_field.empty()) {
		ctx->__headers.insert({std::move(ctx->last_field), std::move(ctx->last_value)});
//		ctx->__headers[std::move(ctx->last_field)] = std::move(ctx->last_value);
	}

	if (ctx->type == HTTP_REQUEST)
		ctx->parse_url();

	ctx->__method.assign(ctx->hp_ctx.method);

	for (auto &it : ctx->__headers) {
		std::string buf = it.first;
		for (auto &it_s : buf) {
			it_s = tolower(it_s);
		}
		ctx->__headers_lowercase.insert({std::move(buf), std::move(std::string_view(it.second.data(), it.second.size()))});
//		ctx->__headers_lowercase[buf] = std::string_view(it.second.data(), it.second.size());
	}

	ctx->__header_finished = true;

	return 0;
}

ssize_t HTTP1::Parser::parse(const char *data, size_t len) {
	auto rc = http_parser_execute(&hp_ctx, &hp_settings, data, len);

	if (rc != len) {
		throw std::invalid_argument(http_errno_name(HTTP_PARSER_ERRNO(&hp_ctx)));
	}

	__size_parsed += len;

	return rc;
}

bool HTTP1::Parser::finished() const noexcept {
	return __finished;
}

bool HTTP1::Parser::headers_finished() const noexcept {
	return __header_finished;
}

void HTTP1::Parser::parse_url() {
	auto buf0 = explode_zerocopy(__url_raw.c_str(), __url_raw.size(), '?');

	__url = std::string(buf0[0].first, buf0[0].first+buf0[0].second);

	if (buf0.size() == 2) {
		auto buf1 = explode_zerocopy(buf0[1].first, buf0[1].second, '&');

		for (auto &it : buf1) {
			auto buf2 = explode_zerocopy(it.first, it.second, '=');

			std::string key, val;

			if (buf2[0].second) {
				key = decodeURIComponent({buf2[0].first, buf2[0].second});
			}

			if (buf2.size() == 2)
				if (buf2[1].second) {
					val = decodeURIComponent({buf2[1].first, buf2[1].second});
				}

			__query_string.insert(std::move(std::pair<std::string, std::string>(std::move(key), std::move(val))));
		}
	}
}

void HTTP1::Parser::init() {
	http_parser_init(&hp_ctx, HTTP_REQUEST);

	hp_ctx.data = this;

	hp_settings.on_url = callback_url;
	hp_settings.on_body = callback_body;
	hp_settings.on_status = callback_status;
	hp_settings.on_header_field = callback_header_field;
	hp_settings.on_header_value = callback_header_value;
	hp_settings.on_headers_complete = callback_headers_complete;
	hp_settings.on_message_complete = callback_message_complete;
}

HTTP1::Parser::Parser() {
	init();
}
