/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Util.hpp"

using namespace Marisa::Util;

static const char str_invalid_date[] = "Invalid Date";

JSDate::JSDate() {
	clock_gettime(CLOCK_REALTIME, &time_);
}

JSDate::JSDate(const std::string &__str) {
	assign(__str);
}

JSDate::JSDate(timespec *__ts) {
	assign_timespec(__ts);
}

void JSDate::assign(const std::string &__str) {
	if (__str.empty()) {
		valid = false;
		return;
	}

	try {
		tm tbuf{0};
		bool sym = true;

		time_t offset = 0;

		if (isupper(__str[0])) { // GMT
			strptime(__str.c_str(), "%a, %d %b %Y %T %Z", &tbuf);
			time_.tv_nsec = 0;
		} else { // ISO mdzz
			auto mpos = __str.find('Z');


			if (mpos == std::string::npos) {
				mpos = __str.find_last_of('+');
				if (mpos == std::string::npos) {
					mpos = __str.find_last_of('-');
					sym = false;
				}
			} else {
				mpos = std::string::npos;
			}

//			printf("mpos: %d\n", mpos);

			if (mpos == std::string::npos) { // No offset
				auto pos = __str.find('.');
				if (pos == std::string::npos) { // Great
					strptime(__str.c_str(), "%Y-%m-%dT%TZ", &tbuf);
					time_.tv_nsec = 0;
				} else { // Yee
					std::string buf(__str.begin(), __str.begin() + pos);
					strptime(buf.c_str(), "%Y-%m-%dT%T", &tbuf);
					std::string buf2(__str.begin() + pos + 1, __str.end());
					time_.tv_nsec = strtol(buf2.c_str(), nullptr, 10) * 1000000;
				}
			} else { // Has offset
				auto pos = __str.find('.');
				if (pos == std::string::npos) { // Great
					strptime(__str.c_str(), "%Y-%m-%dT%TZ", &tbuf);
					time_.tv_nsec = 0;

					std::string buf3(__str.begin() + mpos + 1, __str.end());
					std::string h(buf3.begin(), buf3.begin() + 2);
					std::string m(buf3.begin() + 3, buf3.end());
					offset = strtol(h.c_str(), nullptr, 10) * 3600;
					offset += strtol(m.c_str(), nullptr, 10) * 60;
				} else { // Yee
					std::string buf(__str.begin(), __str.begin() + pos);
					strptime(buf.c_str(), "%Y-%m-%dT%T", &tbuf);

					std::string buf2(__str.begin() + pos + 1, __str.end());
					time_.tv_nsec = strtol(buf2.c_str(), nullptr, 10) * 1000000;

					std::string buf3(__str.begin() + mpos + 1, __str.end());
					std::string h(buf3.begin(), buf3.begin() + 2);
					std::string m(buf3.begin() + 3, buf3.end());
					offset = strtol(h.c_str(), nullptr, 10) * 3600;
					offset += strtol(m.c_str(), nullptr, 10) * 60;
				}
			}

		}

		time_.tv_sec = timegm(&tbuf);

//		printf("offset: %d\n", offset);

		if (sym)
			time_.tv_sec -= offset;
		else
			time_.tv_sec += offset;

	} catch (...) {
		valid = false;
	}
}



std::string JSDate::toGMTString() const noexcept {
	if (!valid)
		return str_invalid_date;

	std::string str_gmt;
	str_gmt.resize(32);
	tm tmbuf{0};
	gmtime_r(&time_.tv_sec, &tmbuf);
	str_gmt.resize(strftime(&str_gmt[0], 32, "%a, %d %b %Y %T GMT", &tmbuf));


	return str_gmt;
}

std::string JSDate::toISOString() const noexcept {
	if (!valid)
		return str_invalid_date;

	std::string str_iso;
	str_iso.resize(32);
	tm tmbuf{0};
	gmtime_r(&time_.tv_sec, &tmbuf);
	str_iso.resize(strftime(&str_iso[0], 32, "%Y-%m-%dT%T", &tmbuf));

	if (time_.tv_nsec) {
		char buf[8];
		snprintf(buf, 7, ".%03d", (int)(time_.tv_nsec / 1000000));
		str_iso += buf;
	}

	str_iso.push_back('Z');


	return str_iso;
}

std::string JSDate::toString() const noexcept {
	if (!valid)
		return str_invalid_date;

	std::string str;
	str.resize(48);
	tm tmbuf{0};
	localtime_r(&time_.tv_sec, &tmbuf);
	str.resize(strftime(&str[0], 48, "%a %b %d %Y %T GMT%z (%Z)", &tmbuf));


	return str;
}

std::string JSDate::toDateString() const noexcept {
	if (!valid)
		return str_invalid_date;

	std::string str_date;
	str_date.resize(32);
	tm tmbuf{0};
	localtime_r(&time_.tv_sec, &tmbuf);
	str_date.resize(strftime(&str_date[0], 32, "%a %b %d %Y", &tmbuf));


	return str_date;
}

std::string JSDate::toGoodString() const noexcept {
	if (!valid)
		return str_invalid_date;

	std::string str_date;
	str_date.resize(32);
	tm tmbuf{0};
	localtime_r(&time_.tv_sec, &tmbuf);
	str_date.resize(strftime(&str_date[0], 32, "%Y-%m-%d %H:%M:%S %z", &tmbuf));

	return str_date;
}


const int64_t JSDate::getTime() const noexcept {
	return valid ? time_.tv_sec * 1000 + (time_.tv_nsec / 1000000) : 0;
}

const time_t JSDate::getTimeSeconds() const noexcept {
	return valid ? time_.tv_sec : 0;
}

void JSDate::assign_unixtime(time_t __t) noexcept {
	valid = true;
	time_.tv_sec = __t;
	time_.tv_nsec = 0;
}

void JSDate::assign_timespec(timespec *__ts) noexcept {
	valid = true;
	time_.tv_sec = __ts->tv_sec;
	time_.tv_nsec = __ts->tv_nsec;
}

void JSDate::assign_jstime(uint64_t __t) noexcept {
	valid = true;
	time_.tv_sec = __t / 1000;
	time_.tv_nsec = (__t % 1000) * 1000000;
}

const timespec& JSDate::get_timespec() const noexcept {
	return time_;
}



