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

#include <sys/types.h>
#include <dirent.h>

#include "CommonIncludes.hpp"

namespace Marisa::Middlewares {
	class StaticFiles : public Middleware {
	MARISA_MIDDLEWARE_USE_DEFAULT_CLONE
	public:
		std::string base_path;
		bool list_files;
		size_t io_buffer_size;

		explicit StaticFiles(const std::string& __base_path, bool __list_files = false, size_t __io_buffer_size = 16384) {
			base_path = __base_path;
			list_files = __list_files;
			io_buffer_size = __io_buffer_size;
		}

		void handler() override;
		void generate_file_page(DIR *__dirp);

	};
}
