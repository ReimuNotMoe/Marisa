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

#include "Mullet.hpp"

using namespace Marisa::Types;

Mullet::Mullet() {
	__parser.userData = this;
	__parser.onPartBegin = onPartBegin;
	__parser.onHeaderField = onHeaderField;
	__parser.onHeaderValue = onHeaderValue;
	__parser.onPartData = onPartData;
	__parser.onPartEnd = onPartEnd;
	__parser.onEnd = onEnd;
}

Mullet::Mullet(const std::string &__data) : Mullet() {
	parse(__data);
}

void Mullet::onPartBegin(const char *buffer, size_t start, size_t end, void *userData) {
	auto ctx = (Mullet *)userData;

	ctx->parts.emplace_back();
}

void Mullet::onHeaderField(const char *buffer, size_t start, size_t end, void *userData) {
	auto ctx = (Mullet *)userData;

	Multipart_Part& current_part = ctx->parts.back();

	if (ctx->last_handler == 2) {
		ctx->last_field.insert(ctx->last_field.end(), buffer+start, buffer+start+(end-start));
	} else {
		if (ctx->last_handler == 3) {
			current_part.headers.insert({std::move(ctx->last_field), std::move(ctx->last_value)});
		}

		ctx->last_handler = 2;
		ctx->last_field.assign(buffer+start, end-start);
	}
}

void Mullet::onHeaderValue(const char *buffer, size_t start, size_t end, void *userData) {
	auto ctx = (Mullet *)userData;

	Multipart_Part& current_part = ctx->parts.back();

	if (ctx->last_handler == 3) {
		ctx->last_value.insert(ctx->last_value.end(), buffer+start, buffer+start+(end-start));
	} else {
		ctx->last_handler = 3;
		ctx->last_value.assign(buffer+start, end-start);
	}
}

void Mullet::onPartData(const char *buffer, size_t start, size_t end, void *userData) {
	auto ctx = (Mullet *)userData;

	Multipart_Part& current_part = ctx->parts.back();

	if (ctx->last_handler == 4) {
		current_part.data.insert(current_part.data.end(), buffer+start, buffer+start+(end-start));
	} else {
		if (ctx->last_handler == 3) {
			current_part.headers.insert({std::move(ctx->last_field), std::move(ctx->last_value)});
		}

		ctx->last_handler = 4;
		current_part.data.assign(buffer+start, end-start);
	}
}

void Mullet::onPartEnd(const char *buffer, size_t start, size_t end, void *userData) {
	auto ctx = (Mullet *)userData;

	Multipart_Part& current_part = ctx->parts.back();

	for (auto &it : current_part.headers) {
		std::string buf;
		buf.assign(it.first);

		for (auto &itc : buf) {
			itc = tolower(itc);
		}

		current_part.headers_lowercase.insert({std::move(buf), it.second});
	}

	current_part.done = true;
}

void Mullet::onEnd(const char *buffer, size_t start, size_t end, void *userData) {
	auto ctx = (Mullet *)userData;
	ctx->__done = true;
}

void Mullet::set_headers(const std::unordered_map<std::string, std::string_view> &__headers) {
	auto ith = __headers.find("content-type");

	if (ith == __headers.end())
		throw std::invalid_argument("no Content-Type found in headers");

	auto &val = ith->second;

	auto pos_mf = val.find("multipart/form-data");
	if (pos_mf == val.npos)
		throw std::invalid_argument("Content-Type is not multipart/form-data");

	auto pos_e = val.find('=');
	if (pos_e == val.npos || pos_e <= pos_mf || pos_e == val.size()-1)
		throw std::invalid_argument("Content-Type is malformed");

	__boundary = val.substr(pos_e+1);
	__parser.setBoundary(__boundary);
}

void Mullet::parse(const std::string_view &__data) {
	__parser.feed(__data.data(), __data.size());
}

void Mullet::parse(const std::vector<uint8_t> &__data) {
	__parser.feed((const char *)__data.data(), __data.size());
}

bool Mullet::done() const noexcept {
	return __done;
}
