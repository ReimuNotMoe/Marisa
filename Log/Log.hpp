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

#ifndef MARISA_LOG_HPP
#define MARISA_LOG_HPP

#include "../CommonIncludes.hpp"

namespace Marisa {
	namespace Log {
		class LogLevel {
		public:
			enum {
				Error = 0x1, Warning = 0x2, Info = 0x4, Debug = 0x8,
				Everything = 0xff
			};
		};

		extern uint8_t Level;
		extern bool Color;

		extern const char AppName[];

		extern void LogD(const char *__fmt, ...) __attribute__ ((format (printf, 1, 2)));
		extern void LogI(const char *__fmt, ...) __attribute__ ((format (printf, 1, 2)));
		extern void LogW(const char *__fmt, ...) __attribute__ ((format (printf, 1, 2)));
		extern void LogE(const char *__fmt, ...) __attribute__ ((format (printf, 1, 2)));
	}
}
#endif //MARISA_LOG_HPP
