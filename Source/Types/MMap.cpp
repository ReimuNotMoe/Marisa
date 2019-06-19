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

#include "MMap.hpp"

using namespace Marisa::Types;

MMap::MMap(const std::string &__path, bool __writable) {
	map_file(__path, __writable);
}

MMap::MMap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	map(addr, length, prot, flags, fd, offset);
}

void MMap::map(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	map_ptr = mmap(addr, length, prot, flags, fd, offset);

	if (map_ptr == MAP_FAILED)
		throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));
}

void MMap::unmap() {
	if (map_ptr != MAP_FAILED)
		if (munmap(map_ptr, map_size))
			throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));
}

void MMap::map_file(const std::string &__path, bool __writable) {
	int fd = open(__path.c_str(), __writable ? O_RDWR : O_RDONLY);

	if (fd == -1)
		throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));

	off_t filesize = lseek(fd, 0, SEEK_END);

	if (filesize == -1)
		throw std::system_error(std::error_code(errno, std::system_category()), strerror(errno));


	map_ptr = mmap(nullptr, filesize, __writable ? PROT_READ | PROT_WRITE : PROT_READ, MAP_SHARED, fd, 0);
	int errno_saved = errno;

	close(fd);

	if (map_ptr == MAP_FAILED)
		throw std::system_error(std::error_code(errno_saved, std::system_category()), strerror(errno_saved));

	map_size = filesize;
}


