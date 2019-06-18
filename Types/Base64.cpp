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

#include "Base64.hpp"

using namespace Marisa::Types::Base64;

Encoder::Encoder() {
	reset();
}

void Encoder::reset() {
	base64_stream_encode_init(&b64_state, 0);
	encoded_size = 0;
}

std::string Encoder::encode(std::string_view __input) {
	std::string ret;
	ret.resize(__input.size() * 4 / 3 + 4);

	base64_stream_encode(&b64_state, __input.data(), __input.size(), ret.data(), &encoded_size);
	ret.resize(encoded_size);

	return ret;
}

std::string Encoder::finalize() {
	std::string ret;
	ret.resize(16);

	base64_stream_encode_final(&b64_state, ret.data(), &encoded_size);
	ret.resize(encoded_size);

	return ret;
}

std::string Encoder::encode_once(std::string_view __input) {
	size_t encoded_size = 0;
	std::string ret;
	ret.resize(__input.size() * 4 / 3 + 4);

	base64_encode(__input.data(), __input.size(), ret.data(), &encoded_size, 0);
	ret.resize(encoded_size);

	return ret;
}

Decoder::Decoder() {
	reset();
}

void Decoder::reset() {
	base64_stream_decode_init(&b64_state, 0);
	decoded_size = 0;
}

std::string Decoder::decode(std::string_view __input) {
	std::string ret;
	ret.resize(__input.size() * 3 / 4 + 4);

	base64_stream_decode(&b64_state, __input.data(), __input.size(), ret.data(), &decoded_size);
	ret.resize(decoded_size);

	return ret;
}

std::string Decoder::decode_once(std::string_view __input) {
	size_t decoded_size = 0;
	std::string ret;
	ret.resize(__input.size() * 3 / 4 + 4);

	base64_decode(__input.data(), __input.size(), ret.data(), &decoded_size, 0);
	ret.resize(decoded_size);

	return ret;
}
