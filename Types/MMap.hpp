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

#ifndef MARISA_TYPE_MMAP_HPP
#define MARISA_TYPE_MMAP_HPP

#include "../CommonIncludes.hpp"

namespace Marisa {
	namespace Types {
		class MMap {
		private:
			void *map_ptr = nullptr;
			size_t map_size = 0;
		public:
			MMap() = default;
			explicit MMap(const std::string& __path, bool __writable = false);
			MMap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
			~MMap() {
				unmap();
			}

			void map(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
			void unmap();

			void map_file(const std::string& __path, bool __writable = false);

			bool empty() const noexcept {
				return map_ptr != nullptr;
			}

			size_t size() const noexcept {
				return map_size;
			}

			void *get() const noexcept {
				return map_ptr;
			}

			explicit operator std::string_view() const noexcept {
				return {(const char *)map_ptr, map_size};
			}

		};
	}
}

#endif //MARISA_MMAP_HPP
