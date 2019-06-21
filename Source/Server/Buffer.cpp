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

#include "Buffer.hpp"

using namespace Marisa::Server;

boost::asio::mutable_buffers_1 Buffer::get() const {
	if (type == 1)
		return boost::asio::buffer(str->data(), str->size());
	else if (type == 2)
		return boost::asio::buffer(vec->data(), vec->size());
	else if (type == 3)
		return boost::asio::buffer((void *)sv->data(), sv->size());
	else
		return boost::asio::buffer((void *)nullptr, 0);
}

size_t Buffer::size() const {
	if (type == 1)
		return str->size();
	else if (type == 2)
		return vec->size();
	else if (type == 3)
		return sv->size();
	else
		return 0;
}

void Buffer::assign(std::string __str) {
	type = 1;
	str = std::make_unique<std::string>(std::move(__str));
}

void Buffer::assign(std::vector<uint8_t> __vec) {
	type = 2;
	vec = std::make_unique<std::vector<uint8_t>>(std::move(__vec));
}

void Buffer::assign(std::string_view __sv) {
	type = 3;
	sv = std::make_unique<std::string_view>(__sv);
}

void Buffer::move_assign(Buffer &o) noexcept {
	type = o.type;

	if (type == 1)
		str = std::move(o.str);
	else if (type == 2)
		vec = std::move(o.vec);
	else if (type == 3)
		sv = std::move(o.sv);
}

void Buffer::assign(const Buffer &o) {
	type = o.type;

	if (type == 1)
		str = std::make_unique<std::string>(*o.str);
	else if (type == 2)
		vec = std::make_unique<std::vector<uint8_t>>(*o.vec);
	else if (type == 3)
		sv = std::make_unique<std::string_view>(*o.sv);
}
