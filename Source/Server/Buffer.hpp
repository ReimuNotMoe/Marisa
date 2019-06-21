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

#ifndef MARISA_SERVER_BUFFER_HPP
#define MARISA_SERVER_BUFFER_HPP

#include "../CommonIncludes.hpp"

// Strive for zero copy!

namespace Marisa {
	namespace Server {
		class Buffer {
		private:

		public:
			Buffer() = default;

			explicit Buffer(std::vector<uint8_t> __vec) {
				assign(std::move(__vec));
			}

			explicit Buffer(std::string __str) {
				assign(std::move(__str));
			}

			explicit Buffer(std::string_view __sv) {
				assign(__sv);
			}

			Buffer(const Buffer& o) {
				assign(o);
			}

			Buffer(Buffer&& o) noexcept {
				move_assign(o);
			}

			Buffer& operator= (const Buffer& o) {
				assign(o);
				return *this;
			}

			Buffer& operator= (Buffer&& o) noexcept {
				move_assign(o);
				return *this;
			}

			int type = 0;

			std::unique_ptr<std::vector<uint8_t>> vec;
			std::unique_ptr<std::string> str;
			std::unique_ptr<std::string_view> sv;

			void assign(const Buffer& o);

			void move_assign(Buffer& o) noexcept;

			void assign(std::vector<uint8_t> __vec);
			void assign(std::string __str);
			void assign(std::string_view __sv);

			boost::asio::mutable_buffers_1 get() const;
			size_t size() const;
		};
	}
}



#endif //MARISA_BUFFER_HPP
