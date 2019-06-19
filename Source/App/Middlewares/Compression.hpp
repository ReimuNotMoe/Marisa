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

#ifndef MARISA_APPMW_COMPRESSION_HPP
#define MARISA_APPMW_COMPRESSION_HPP

#include "CommonIncludes.hpp"
#include "../Response/Response.hpp"

namespace Marisa::Application::Middlewares {

	enum CompFlags : uint16_t {
		GZIP = 0x1, DEFLATE = 0x2,
		LEVEL_DEFAULT = 0x4, LEVEL_FASTEST = 0x8, LEVEL_BEST = 0x10,
		STRATEGY_FILTERED = 0x20, STRATEGY_HUFFMAN_ONLY = 0x40, STRATEGY_FIXED = 0x80, STRATEGY_RLE = 0x100,
		DEFAULT = GZIP|DEFLATE|LEVEL_BEST
	};

	class __compression_response_overrider : public Response::ResponseContext {
	protected:
		CompFlags settings;
		Request::RequestContext *request = nullptr;
		bool zlib_ctx_present = false;


		std::string buf;
		size_t buf_size;
		z_stream zlib_strm;

	public:

		__compression_response_overrider(Context *__context, CompFlags __f, Request::RequestContext *__req, size_t __buf_size);

		void decide_encoding();
//		void finish_encoding();

		void send(std::string __s, bool __blocking = false) override;
		void write(std::string __s, bool __blocking = false) override;
		void end() override;

		~__compression_response_overrider() override;

	};

	class Compression : public Middleware {
	protected:
		CompFlags settings = CompFlags::DEFAULT;
		size_t buf_size = 4096;

	public:
		Compression() = default;
		explicit Compression(CompFlags __f, size_t __buf_size = 4096) {
			settings = __f;
			buf_size = __buf_size;
		}

		void handler() override;

		std::unique_ptr<Middleware> New() const override {
			return std::make_unique<Compression>(settings, buf_size);
		}
	};
}

#endif //MARISA_COMPRESSION_HPP
