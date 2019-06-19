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

#include "Body.hpp"
#include "Response.hpp"

using namespace Marisa::Application::Response;

void Body::set_content_type(const char *__type) {
//	if (resp_ctx.type.empty())
	resp_ctx.type = __type;
}

Body& Body::operator=(const std::string &rhs) {
	assign_string(rhs);

	return *this;
}

Body &Body::operator=(const char *rhs) {
	type = 1;
	if (resp_ctx.type.empty())
		resp_ctx.type = "text/html";
	if (!str)
		str = std::make_unique<std::string>(rhs);
	else
		*str = rhs;

	return *this;
}


void Body::assign_string(const std::string &s) {
	type = 1;
	set_content_type("text/html");

	if (!str)
		str = std::make_unique<std::string>(s);
	else
		*str = s;
}

void Body::assign_cstring(const char *s) {
	type = 1;
	set_content_type("text/html");

	if (!str)
		str = std::make_unique<std::string>(s);
	else
		*str = s;
}

void Body::assign_cstring(const char *s, size_t l) {
	type = 1;
	set_content_type("text/html");

	if (!str)
		str = std::make_unique<std::string>();

	str->clear();
	str->insert(str->end(), s, s+l);
}

void Body::assign_data(const char *s, size_t l) {
	type = 1;
	set_content_type("application/octet-stream");

	if (!str)
		str = std::make_unique<std::string>();

	str->clear();
	str->insert(str->end(), s, s+l);
}

void Body::emplace_string(std::string &s) {
	type = 1;
	set_content_type("text/html");

	if (!str)
		str = std::make_unique<std::string>(s);
	else
		*str = std::move(s);
}

void Body::emplace_cstring(const char *s) {
	type = 2;
	set_content_type("text/html");

	str = std::unique_ptr<std::string>();

	cstr = s;
	cstr_len = strlen(s);
}

void Body::emplace_cstring(const char *s, size_t l) {
	type = 2;
	set_content_type("text/html");

	str = std::unique_ptr<std::string>();

	cstr = s;
	cstr_len = l;
}

void Body::emplace_data(const char *s, size_t l) {
	type = 2;
	set_content_type("application/octet-stream");

	str = std::unique_ptr<std::string>();

	cstr = s;
	cstr_len = l;
}

std::string_view Body::get() {
	if (type == 1)
		return *str;
	else if (type == 2)
		return {cstr, cstr_len};
	else
		return "";
}

