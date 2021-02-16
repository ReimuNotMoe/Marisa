/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../CommonIncludes.hpp"

namespace Marisa {
	namespace Util {
		class SpinLock {
			std::atomic_flag flag = ATOMIC_FLAG_INIT;
		public:
			void lock() {
				while (flag.test_and_set(std::memory_order_acquire));
			}

			void unlock() {
				flag.clear(std::memory_order_release);
			}
		};

		template<typename T>
		static void explode_string(const std::string &s, char delim, T result) {
			std::stringstream ss(s);
			std::string item;
			while (std::getline(ss, item, delim)) {
				*(result++) = item;
			}
		}

		std::string read_file(const std::string& path);

		long cpus_available();
		size_t memory_usage();

		const std::string& mime_type(std::string dot_file_ext);

		const std::string& default_status_page(int status);

		std::string encodeURI(const std::string_view &__str);
		std::string encodeURIComponent(const std::string_view &__str);
		size_t decodeURI(const char *src, size_t src_size, char *dst);
		size_t decodeURIComponent(const char *src, size_t src_size, char *dst);
		std::string decodeURI(const std::string_view &__str);
		std::string decodeURIComponent(const std::string_view &__str);



	}
}
