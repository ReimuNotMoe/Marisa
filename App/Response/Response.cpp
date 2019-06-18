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

#include "Response.hpp"
#include "../Context.hpp"
#include "../../Types/MMap.hpp"

using namespace Marisa::Application::Response;
using namespace Marisa::Types;

ResponseContext::ResponseContext(Context *__context) {
	context = __context;
}

ResponseContext::~ResponseContext() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdestructor called\n", "ResponseCtx", (uintptr_t)this);
#endif
}

static const std::string chunk_end = "\r\n";
static const std::string chunk_all_end = "0\r\n\r\n";

static inline std::string chunk_header(size_t __size) {
	std::string ret;
	ret.resize(16);
	snprintf((char *)ret.data(), 15, "%lX\r\n", __size);
	ret.resize(strlen((const char *)ret.data()));

	return ret;
}

void ResponseContext::send_headers(uint16_t flags) {
	if (!headers_sent) {
		auto ce = static_cast<ContextExposed *>(context);
		auto sess = ce->session;
		headers_sent = true;

		if (flags & 0x1) {
			auto &ref_te = headers["Transfer-Encoding"];
			if (ref_te.empty())
				ref_te = "chunked";
			else
				ref_te += ",chunked";
		}

		headers["Content-Type"] = std::move(type);

		sess->async_write(std::move(ce->http_generator->generate_headers(*this)));
	}
}

void ResponseContext::write(std::string __s, bool __blocking) {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite called, size=%zu\n", "ResponseCtx", (uintptr_t)this, __s.size());
#endif

	send_headers(0x1);

	buf_write = std::move(__s);

	if (__blocking) {
		sess->async_write(std::move(chunk_header(buf_write.size())));
		sess->blocking_write(std::move(buf_write));
		sess->async_write(chunk_end);
	} else {
		sess->async_write(std::move(chunk_header(buf_write.size())));
		sess->async_write(std::move(buf_write));
		sess->async_write(chunk_end);
	}
}

void ResponseContext::raw_write(std::string __s, bool __blocking) {
	// Headers shouldn't be processed here due to incomplete Content-Length
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	buf_write = std::move(__s);

	if (__blocking)
		sess->blocking_write(std::move(buf_write));
	else
		sess->async_write(std::move(buf_write));
}

void ResponseContext::end() {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	sess->async_write(chunk_all_end);
}

void ResponseContext::send(std::string __s, bool __blocking) {
	headers["Content-Length"] = std::to_string(__s.size());
	send_headers();
	raw_write(std::move(__s), __blocking);
}

void ResponseContext::send_file(const std::string &__file_path, size_t __buffer_size) {
	MMap fmap(__file_path);

	auto fptr = (uint8_t *)fmap.get();
	auto fsize = fmap.size();

	headers["Content-Length"] = std::to_string(fsize);
	headers["Content-Encoding"] = "identity";
	send_headers();

	std::string buf;

	for (size_t pos=0; pos<fsize; ) {
		size_t size_left = fsize - pos;
		size_t size_write = size_left > __buffer_size ? __buffer_size : size_left;

		buf.assign((const char *)fptr+pos, size_write); // Fastest way, resize() will memset zero, insert() is slow as hell
//		memcpy(buf.data(), fptr+pos, size_write);
		raw_write(std::move(buf), true);

		pos += size_write;
	}
}
