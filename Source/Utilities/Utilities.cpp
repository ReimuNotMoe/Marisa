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

#include "Utilities.hpp"

static const char http_status_part0[] = "<!DOCTYPE html>\n"
					"<html>\n"
					"<head><title>";

static const char http_status_part1[] = "</title></head>\n"
					"<body bgcolor=\"white\">\n"
					"<center><h1>";

static const char http_status_part2[] = "</h1></center>\n"
					"<hr><center>Marisa/0.1</center>\n"
					"</body>\n"
					"</html>";

using namespace Marisa;



std::vector<std::string> Utilities::explode_string(const std::string &s, char delim) {
	std::vector<std::string> ret;
	explode_string(s, delim, std::back_inserter(ret));
	return ret;
}

std::vector<std::pair<char *, size_t>> Utilities::explode_zerocopy(const char *haystack, size_t haystack_len, char needle) {
	std::vector<std::pair<char *, size_t>> ret;

	const char *last_pos = haystack;
	const char *pos = haystack;

	while (pos < haystack+haystack_len) {
		pos = (const char *)memchr((const void *)pos, needle, haystack_len-(pos-haystack));
		if (!pos)
			pos = haystack+haystack_len;

		ret.emplace_back(std::pair<char *, size_t>((char *)last_pos, (size_t)(pos-last_pos)));
		pos++;
		last_pos = pos;
	}

	if (pos[-1] == needle)
		ret.emplace_back(std::pair<char *, size_t>((char *)pos, (size_t)0));

	return ret;
}

std::vector<std::string_view> Utilities::explode_zerocopy_sv(const char *haystack, size_t haystack_len, char needle) {
	std::vector<std::string_view> ret;

	const char *last_pos = haystack;
	const char *pos = haystack;

	while (pos < haystack+haystack_len) {
		pos = (const char *)memchr((const void *)pos, needle, haystack_len-(pos-haystack));
		if (!pos)
			pos = haystack+haystack_len;

		ret.emplace_back(std::string_view((char *)last_pos, (size_t)(pos-last_pos)));
		pos++;
		last_pos = pos;
	}

	if (pos[-1] == needle)
		ret.emplace_back(std::string_view((char *)pos, (size_t)0));

	return ret;
}

std::string Utilities::http_status_page(const HTTP::Status &status) {
	std::string ret;
	std::string sbuf = std::to_string(status) + " ";
	sbuf += status.to_string();

	ret.insert(ret.end(), http_status_part0, http_status_part0 + sizeof(http_status_part0) -1);

	ret += sbuf;

	ret.insert(ret.end(), http_status_part1, http_status_part1 + sizeof(http_status_part1) -1);

	ret += sbuf;

	ret.insert(ret.end(), http_status_part2, http_status_part2 + sizeof(http_status_part2) -1);

	return ret;
}


std::vector<uint8_t> Utilities::http_status_page_v(const HTTP::Status &status) {
	std::vector<uint8_t> ret(1024);

	http_status_page_v(ret, status);

	return ret;
}

void Utilities::http_status_page_v(std::vector<uint8_t> &v, const HTTP::Status &status) {
	std::string sbuf = std::to_string(status) + " ";
	sbuf += status.to_string();

	v.insert(v.end(), http_status_part0, http_status_part0 + sizeof(http_status_part0) - 1);
	v.insert(v.end(), sbuf.c_str(), sbuf.c_str() + sbuf.size());
	v.insert(v.end(), http_status_part1, http_status_part1 + sizeof(http_status_part1) - 1);
	v.insert(v.end(), sbuf.c_str(), sbuf.c_str() + sbuf.size());
	v.insert(v.end(), http_status_part2, http_status_part2 + sizeof(http_status_part2) - 1);
}

std::string Utilities::date_string() {
	char buf[30];

	auto unix_time = time(nullptr);
	tm local_time;
	localtime_r(&unix_time, &local_time);

	strftime(buf, 29, "[%Y-%m-%d %H:%M:%S %z]", &local_time);

	return std::string(buf);
}

long Utilities::cpus_available() {
	long nprocs = 1;
#ifdef _WIN32
	#ifndef _SC_NPROCESSORS_ONLN
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	#define sysconf(a) info.dwNumberOfProcessors
	#define _SC_NPROCESSORS_ONLN
	#endif
#endif

#ifdef _SC_NPROCESSORS_ONLN
	nprocs = sysconf(_SC_NPROCESSORS_ONLN);
#endif

	return nprocs;
}

