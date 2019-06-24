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
	auto ce = static_cast<ContextExposed *>(context);
	session = ce->session;
}

ResponseContext::~ResponseContext() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdestructor called\n", "ResponseCtx", (uintptr_t)this);
#endif
}

static const std::string chunk_end_str = "\r\n";
static const std::string chunk_all_end_str = "0\r\n\r\n";

static const Buffer chunk_all_end(std::string_view(chunk_all_end_str.data(), chunk_all_end_str.size()));
static const Buffer chunk_end(std::string_view(chunk_end_str.data(), chunk_end_str.size()));

static inline Buffer chunk_header(size_t __size) {
	Buffer ret;
	ret.vec = std::make_unique<std::vector<uint8_t>>(16);
	ret.type = 2;
	snprintf((char *)ret.vec->data(), 15, "%lX\r\n", __size);
	ret.vec->resize(strlen((const char *)ret.vec->data()));

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

		if (yield_context)
			sess->write_async(std::move(Buffer(std::move(ce->http_generator->generate_headers(*this)))), *yield_context);
		else
			sess->write_promised(std::move(Buffer(std::move(ce->http_generator->generate_headers(*this)))));
	}
}

void ResponseContext::do_write(bool __blocking) {
	if (yield_context) {
		session->write_async((chunk_header(buf_write.size())), *yield_context);
		session->write_async(std::move(buf_write), *yield_context);
		session->write_async(chunk_end, *yield_context);
	} else {
		if (__blocking) {
			session->write_promised((chunk_header(buf_write.size())));
			session->write_blocking(std::move(buf_write));
			session->write_promised(chunk_end);
		} else {
			session->write_promised(std::move(chunk_header(buf_write.size())));
			session->write_promised(std::move(buf_write));
			session->write_promised(chunk_end);
		}
	}
}


void ResponseContext::write(std::string __s, bool __blocking) {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite called, size=%zu\n", "ResponseCtx", (uintptr_t)this, __s.size());
#endif

	send_headers(0x1);

	buf_write.assign(std::move(__s));

	do_write(__blocking);
}


void ResponseContext::write(std::vector<uint8_t> __s, bool __blocking) {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite called, size=%zu\n", "ResponseCtx", (uintptr_t)this, __s.size());
#endif

	send_headers(0x1);

	buf_write.assign(std::move(__s));

	do_write(__blocking);
}

void ResponseContext::write(const void *__s, size_t __len, bool __blocking) {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\twrite called, size=%zu\n", "ResponseCtx", (uintptr_t)this, __s.size());
#endif

	send_headers(0x1);

	buf_write.assign(std::string_view((const char *)__s, __len));

	do_write(__blocking);
}


void ResponseContext::do_raw_write(bool __blocking) {
	if (yield_context) {
		session->write_async(std::move(buf_write), *yield_context);
	} else {
		if (__blocking)
			session->write_blocking(std::move(buf_write));
		else
			session->write_promised(std::move(buf_write));
	}
}


void ResponseContext::raw_write(std::string __s, bool __blocking) {
	// Headers shouldn't be processed here due to incomplete Content-Length
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	buf_write.assign(std::move(__s));

	do_raw_write(__blocking);
}

void ResponseContext::raw_write(std::vector<uint8_t> __s, bool __blocking) {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	buf_write.assign(std::move(__s));

	do_raw_write(__blocking);
}

void ResponseContext::raw_write(const void *__s, size_t __len, bool __blocking) {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	buf_write.assign(std::string_view((const char *)__s, __len));

	do_raw_write(__blocking);
}

void ResponseContext::end() {
	auto ce = static_cast<ContextExposed *>(context);
	auto sess = ce->session;

	if (yield_context)
		sess->write_async(chunk_all_end, *yield_context);
	else
		sess->write_promised(chunk_all_end);
}

void ResponseContext::send(std::string __s, bool __blocking) {
	headers["Content-Length"] = std::to_string(__s.size());
	send_headers();
	raw_write(std::move(__s), __blocking);
}

void ResponseContext::send(std::vector<uint8_t> __s, bool __blocking) {
	headers["Content-Length"] = std::to_string(__s.size());
	send_headers();
	raw_write(std::move(__s), __blocking);
}

void ResponseContext::send(const void *__s, size_t __len, bool __blocking) {
	headers["Content-Length"] = std::to_string(__len);
	send_headers();
	raw_write(__s, __len, __blocking);
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






