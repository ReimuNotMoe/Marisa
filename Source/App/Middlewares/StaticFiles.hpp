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

#pragma once

#include <sys/types.h>
#include <dirent.h>

#include "CommonIncludes.hpp"

namespace Marisa::Application::Middlewares {
	class StaticFiles : public Middleware {
	public:
		std::string base_path;
		bool list_files;
		size_t path_smatch_pos;
		size_t io_buffer_size;

		explicit StaticFiles(const std::string& __base_path, bool __list_files = false, size_t __io_buffer_size = 16384, size_t __path_smatch_pos = 2) {
			base_path = __base_path;
			list_files = __list_files;
			path_smatch_pos = __path_smatch_pos;
			io_buffer_size = __io_buffer_size;
		}

		void handler() override;
		void generate_file_page(DIR *__dirp);

		std::unique_ptr<Middleware> New() const override {
			return std::make_unique<StaticFiles>(base_path, list_files, io_buffer_size, path_smatch_pos);
		}
	};
}
