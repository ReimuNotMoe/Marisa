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

// Deprecated, keep for future use

namespace Marisa {
	namespace Server {
		class Buffer {
		private:
			static void buf_deleter(uint8_t *p) {
				if (*p) {
					auto *s = *(std::string **)(p+1);
					delete s;
				} else {
					auto *v = *(std::vector<uint8_t> **)(p+1);
					delete v;
				}

				delete[] p;
			}

			std::deque<std::unique_ptr<uint8_t, decltype(&buf_deleter)>> super_buffer;
			std::deque<std::vector<uint8_t>> buf;
			size_t pos = 0;

			static std::unique_ptr<uint8_t, decltype(&buf_deleter)> new_buf(uint8_t type, void *ptr) {
				auto buf = new uint8_t[sizeof(void *) + 1];
				buf[0] = type;
				auto pptr = (void **)(buf+1);
				pptr[0] = ptr;

				return {buf, &buf_deleter};
			}



		public:
			Buffer() = default;

			void Enqueue(void *__buf, size_t __len) {
				auto vec = new std::vector<uint8_t>((uint8_t *)__buf, (uint8_t *)__buf + __len);
				super_buffer.emplace_back(new_buf(0, vec));
			}

			void Enqueue(const std::vector<uint8_t> &__buf) {
				auto vec = new std::vector<uint8_t>(__buf);
				super_buffer.emplace_back(new_buf(0, vec));
			}

			void Enqueue(const std::string &__buf) {
				auto str = new std::string(__buf);
				super_buffer.emplace_back(new_buf(1, str));
			}

			std::pair<ssize_t, int> Write(int fd) {
				if (super_buffer.empty())
					return {-2, -2};


				const uint8_t *cbuf = nullptr;
				size_t csize = 0;

				auto it = super_buffer.begin();
				auto ptr = it->get();

				if (ptr[0]) {
					auto *s = *(std::string **)(ptr+1);
					cbuf = (const uint8_t *)s->c_str();
					csize = s->size();
				} else {
					auto *v = *(std::vector<uint8_t> **)(ptr+1);
					cbuf = (const uint8_t *)v->data();
					csize = v->size();
				}

				size_t cleft = csize - pos;

				ssize_t rc = write(fd, cbuf + pos, cleft);
				int buf_err = errno;

				if ((size_t)rc == cleft) {
					super_buffer.erase(super_buffer.begin());
					pos = 0;
				} else if (rc > 0) {
					pos += rc;
				}

				return {rc, buf_err};
			}

//			void Trim() {
//				buf.erase(buf.begin(), buf.begin() + pos);
//			}
//
//			size_t RemainingLength() {
//				return buf.size() - pos;
//			}
		};
	}
}



#endif //MARISA_BUFFER_HPP
