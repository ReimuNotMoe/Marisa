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

#include "Log.hpp"
#include "../Utilities/Utilities.hpp"

using namespace Marisa::Log;
using namespace Marisa::Utilities;

uint8_t Marisa::Log::Level = LogLevel::Everything;
bool Marisa::Log::Color = true;

const char Marisa::Log::AppName[] = "Marisa";

static const char color_end[] = "\e[0m";
static const char color_lightblue[] = "\e[01;36m";
static const char color_purple[] = "\e[01;35m";
static const char color_yellow[] = "\e[01;33m";
static const char color_green[] = "\e[01;32m";
static const char color_red[] = "\e[01;31m";

static inline std::string datestr_wrapper() {
	std::string buf;
	if (Color)
		buf += color_lightblue;
	buf += date_string();
	if (Color)
		buf += color_end;

	return buf;
}

void Marisa::Log::LogD(const char *__fmt, ...) {
	if (Level & LogLevel::Debug) {
		std::string buf = datestr_wrapper();
		if (Color)
			buf += color_purple;
		buf += " Debug: ";
		if (Color)
			buf += color_end;
		buf += __fmt;

		va_list ap;
		va_start(ap, __fmt);
		vfprintf(stderr, buf.c_str(), ap);
		va_end(ap);
	}
}

void Marisa::Log::LogI(const char *__fmt, ...) {
	if (Level & LogLevel::Info) {
		std::string buf = datestr_wrapper();
		if (Color)
			buf += color_green;
		buf += " Info:  ";
		if (Color)
			buf += color_end;
		buf += __fmt;

		va_list ap;
		va_start(ap, __fmt);
		vfprintf(stderr, buf.c_str(), ap);
		va_end(ap);
	}
}

void Marisa::Log::LogW(const char *__fmt, ...) {
	if (Level & LogLevel::Warning) {
		std::string buf = datestr_wrapper();
		if (Color)
			buf += color_yellow;
		buf += " Warn:  ";
		if (Color)
			buf += color_end;
		buf += __fmt;

		va_list ap;
		va_start(ap, __fmt);
		vfprintf(stderr, buf.c_str(), ap);
		va_end(ap);
	}
}

void Marisa::Log::LogE(const char *__fmt, ...) {
	if (Level & LogLevel::Error) {
		std::string buf = datestr_wrapper();
		if (Color)
			buf += color_red;
		buf += " Error: ";
		if (Color)
			buf += color_end;
		buf += __fmt;

		va_list ap;
		va_start(ap, __fmt);
		vfprintf(stderr, buf.c_str(), ap);
		va_end(ap);
	}
}