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

#include "Compression.hpp"

using namespace Middlewares;

void Compression::handler() {
	context->response = std::make_unique<__compression_response_overrider>(context, settings, request, buf_size);

	next();
}

void compress_memory(void *in_data, size_t in_data_size, std::vector<uint8_t> &out_data) {
	std::vector<uint8_t> buffer;

	const size_t BUFSIZE = 128 * 1024;
	uint8_t temp_buffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = nullptr;
	strm.zfree = nullptr;
	strm.next_in = reinterpret_cast<uint8_t *>(in_data);
	strm.avail_in = in_data_size;
	strm.next_out = temp_buffer;
	strm.avail_out = BUFSIZE;

	deflateInit(&strm, Z_BEST_COMPRESSION);

	while (strm.avail_in != 0)
	{
		int res = deflate(&strm, Z_NO_FLUSH);
		assert(res == Z_OK);
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
	}

	int deflate_res = Z_OK;
	while (deflate_res == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
		deflate_res = deflate(&strm, Z_FINISH);
	}

	assert(deflate_res == Z_STREAM_END);
	buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);

	out_data.swap(buffer);
}

void __compression_response_overrider::compress_once(const void *__data, size_t __len) {
	zlib_strm.next_in = (Bytef *)__data;
	zlib_strm.avail_in = __len;
	zlib_strm.next_out = (unsigned char *)buf.data();
	zlib_strm.avail_out = buf_size;


	while (zlib_strm.avail_in != 0) {
		int res = deflate(&zlib_strm, Z_NO_FLUSH);
		assert(res == Z_OK);

		if (zlib_strm.avail_out == 0) {
			auto oldsize = buf.size();
			buf.resize(oldsize+buf_size);
			zlib_strm.avail_out = buf_size;
			zlib_strm.next_out = (unsigned char *)(buf.data() + oldsize);
		}
	}

	int deflate_res = Z_OK;
	while (deflate_res == Z_OK) {
		if (zlib_strm.avail_out == 0) {
			auto oldsize = buf.size();
			buf.resize(oldsize+buf_size);
			zlib_strm.avail_out = buf_size;
			zlib_strm.next_out = (unsigned char *)(buf.data() + oldsize);
		}
		deflate_res = deflate(&zlib_strm, Z_FINISH);
	}

	assert(deflate_res == Z_STREAM_END);
	deflateEnd(&zlib_strm);
	zlib_ctx_present = false;

	buf.resize(buf.size()-zlib_strm.avail_out);
}

void __compression_response_overrider::send(std::string __s, bool __blocking) {
	if (!zlib_ctx_present) {
		ResponseContext::send(std::move(__s), __blocking);
		return;
	}

	compress_once(__s.data(), __s.size());

	ResponseContext::send(std::move(buf), __blocking);
}

void __compression_response_overrider::send(std::vector<uint8_t> __s, bool __blocking) {
	if (!zlib_ctx_present) {
		ResponseContext::send(std::move(__s), __blocking);
		return;
	}

	compress_once(__s.data(), __s.size());

	ResponseContext::send(std::move(buf), __blocking);
}

void __compression_response_overrider::send(const void *__s, size_t __len, bool __blocking) {
	if (!zlib_ctx_present) {
		ResponseContext::send(std::move(__s), __blocking);
		return;
	}

	compress_once(__s, __len);

	ResponseContext::send(std::move(buf), __blocking);
}

void __compression_response_overrider::compress_stream(const void *__data, size_t __len, bool __blocking) {
	zlib_strm.next_in = (Bytef *)__data;
	zlib_strm.avail_in = __len;
	zlib_strm.next_out = (unsigned char *)buf.data();
	zlib_strm.avail_out = buf_size;


	while (zlib_strm.avail_in != 0) {
		int res = deflate(&zlib_strm, Z_NO_FLUSH);
		assert(res == Z_OK);

		if (zlib_strm.avail_out == 0) {
			ResponseContext::write(std::move(buf));
			buf.resize(buf_size);
			zlib_strm.next_out = (unsigned char *)buf.data();
			zlib_strm.avail_out = buf_size;
		}
	}


	size_t processed_size = buf_size - zlib_strm.avail_out;
	if (processed_size){
		buf.resize(processed_size);
		ResponseContext::write(std::move(buf), __blocking);
		buf.resize(buf_size);
	}
}

void __compression_response_overrider::write(std::string __s, bool __blocking) {
	if (!zlib_ctx_present) {
		ResponseContext::write(std::move(__s), __blocking);
		return;
	}

	compress_stream(__s.data(), __s.size(), __blocking);
}

void __compression_response_overrider::write(std::vector<uint8_t> __s, bool __blocking) {
	if (!zlib_ctx_present) {
		ResponseContext::write(std::move(__s), __blocking);
		return;
	}

	compress_stream(__s.data(), __s.size(), __blocking);
}

void __compression_response_overrider::write(const void *__s, size_t __len, bool __blocking) {
	if (!zlib_ctx_present) {
		ResponseContext::write(std::move(__s), __blocking);
		return;
	}

	compress_stream(__s, __len, __blocking);
}

void __compression_response_overrider::end() {
	if (zlib_ctx_present) {
		zlib_strm.next_out = (unsigned char *) buf.data();
		zlib_strm.avail_out = buf_size;

		int deflate_res = Z_OK;
		while (deflate_res == Z_OK) {
			if (zlib_strm.avail_out == 0) {
				ResponseContext::write(std::move(buf));
				buf.resize(buf_size);
				zlib_strm.next_out = (unsigned char *) buf.data();
				zlib_strm.avail_out = buf_size;
			}
			deflate_res = deflate(&zlib_strm, Z_FINISH);
		}

		assert(deflate_res == Z_STREAM_END);

		size_t processed_size = buf_size - zlib_strm.avail_out;
		if (processed_size) {
			buf.resize(processed_size);
			ResponseContext::write(std::move(buf));
			buf.resize(buf_size);
		}

		deflateEnd(&zlib_strm);
	}

	zlib_ctx_present = false;
	ResponseContext::end();
}

void __compression_response_overrider::decide_encoding() {
	auto it = request->headers_lowercase.find("accept-encoding");
	if (it == request->headers_lowercase.end()) {
		return;
	}

	std::string cttype;
	int level, strategy = 0;
	int rc = Z_OK;

	if (settings & CompFlags::LEVEL_FASTEST)
		level = Z_BEST_SPEED;
	else if (settings & CompFlags::LEVEL_DEFAULT)
		level = Z_DEFAULT_COMPRESSION;
	else
		level = Z_BEST_COMPRESSION;

	if (settings & CompFlags::STRATEGY_FIXED)
		strategy = Z_FIXED;
	else if (settings & CompFlags::STRATEGY_FILTERED)
		strategy = Z_FILTERED;
	else if (settings & CompFlags::STRATEGY_HUFFMAN_ONLY)
		strategy = Z_HUFFMAN_ONLY;
	else if (settings & CompFlags::STRATEGY_RLE)
		strategy = Z_RLE;

	auto &s = it->second;

	if (s[0] == 'g') {
		if (settings & CompFlags::GZIP) {
			rc = deflateInit2(&zlib_strm, level, Z_DEFLATED, 15|16, 8, strategy);
			cttype = "gzip";
		} else if (settings & CompFlags::DEFLATE) {
			if (s.find('d') != s.npos) {
				rc = deflateInit2(&zlib_strm, level, Z_DEFLATED, 15, 8, strategy);
				cttype = "deflate";
			}
		}
	} else if (s[0] == 'd') {
		if (settings & CompFlags::DEFLATE) {
			rc = deflateInit2(&zlib_strm, level, Z_DEFLATED, 15, 8, strategy);
			cttype = "deflate";
		} else if (settings & CompFlags::GZIP) {
			if (s.find('g') != s.npos) {
				rc = deflateInit2(&zlib_strm, level, Z_DEFLATED, 15|16, 8, strategy);
				cttype = "gzip";
			}
		}
	}

	if (rc != Z_OK)
		throw std::system_error(std::error_code(rc, std::generic_category()), "zlib init error");

	if (!cttype.empty()) {
		zlib_ctx_present = true;
		headers["Content-Encoding"] = std::move(cttype);
	}
}

__compression_response_overrider::__compression_response_overrider(Context *__context,
										Middlewares::CompFlags __f,
										Request::RequestContext *__req,
										size_t __buf_size) :
	ResponseContext(__context) {
	settings = __f;
	request = __req;
	buf_size = __buf_size;
	buf.resize(buf_size);
	zlib_strm.zalloc = nullptr;
	zlib_strm.zfree = nullptr;
	zlib_strm.next_out = (unsigned char *)buf.data();
	zlib_strm.avail_out = buf_size;

	decide_encoding();
}

__compression_response_overrider::~__compression_response_overrider() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdestructor called\n", "Compression", (uintptr_t)this);
#endif
	if (zlib_ctx_present)
		deflateEnd(&zlib_strm);
}


