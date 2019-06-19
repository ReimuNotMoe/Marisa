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

#include "StaticFiles.hpp"

static const char ModuleName[] = "StaticFiles";

void Middlewares::StaticFiles::handler() {
	auto path = decodeURIComponent(request->url_smatch[path_smatch_pos]);

	// Get rid of script kiddies
	if (path.find("..") != path.npos) {
		response->status = 200;
		response->send("You are a teapot");
		return;
	}

	auto full_path = base_path + "/" + path;

	struct stat stat_buf;
	if (stat(full_path.c_str(), &stat_buf)) {
		if (errno == ENOENT) {
			response->status = 404;
		} else if (errno == EPERM) {
			response->status = 403;
		} else {
			response->status = 500;
		}

		response->send(http_status_page(response->status));
		return;
	}

	DIR *dirp = nullptr;

	if ((dirp = opendir((full_path + "/").c_str()))) { // Directory
		if (list_files) {
			generate_file_page(dirp);
		} else {
			response->status = 403;
			response->send(http_status_page(response->status));
		}

		closedir(dirp);
	} else { // File
		if (errno == ENOENT) {
			response->status = 404;
		} else if (errno == EPERM) {
			response->status = 403;
		} else if (errno == ENOTDIR) {
			try {
				auto dot_pos = path.find_last_of('.');
				if (dot_pos == path.npos)
					response->headers["Content-Type"] = "application/octet-stream";
				else
					response->headers["Content-Type"] = get_mime_type(path.substr(dot_pos+1));

				response->send_file(full_path);
				return;
			} catch (std::system_error& e) {
				LogE("%s[0x%016" PRIxPTR "]:\tsend_file error: %s\n", ModuleName, (uintptr_t)this, e.what());
				response->status = 500;
				response->send(http_status_page(response->status));
				return;
			}
		} else {
			response->status = 500;
		}
		response->send(http_status_page(response->status));
		return;
	}
}

void Middlewares::StaticFiles::generate_file_page(DIR *__dirp) {
	errno = 0;

	std::stringstream ss;

	std::string title = "Index of " + request->url;

	ss << "<!DOCTYPE HTML>"
	      "<html>"
	      "<head>"
	      "<title>"
	   << title
	   << "</title>"
	      "<style>"
	      "html {"
	      "font-family: Monospace;"
	      "font-size: 16px;"
	      "}"
	      "</style>"
	      "</head>"
	      "<body>"
	      "<h1>"
	   << title
	   << "</h1>"
	      "<table style=\"text-align: left;\" border=\"1\">"
	      "<tr>"
	      "<th>Type</th>"
	      "<th>Name</th>"
	      "<th>Last modified</th>"
	      "<th>Size</th>"
	      "</tr>";

	struct dirent *de = nullptr;
	struct stat sbuf;

	while ((de = readdir(__dirp))) {
		if (strcmp(de->d_name, ".") == 0)
			continue;

		auto full_path = base_path + "/" + de->d_name;
		stat(full_path.c_str(), &sbuf);

		ss << "<tr>"
		      "<th valign=\"top\">";

		if (de->d_type == DT_DIR)
			ss << "DIR";
		else if (de->d_type == DT_REG)
			ss << "REG";
		else if (de->d_type == DT_LNK)
			ss << "LNK";

		ss << "</th>"
		      "<th><a href=\"./";

		ss << encodeURIComponent(de->d_name);
		if (de->d_type == DT_DIR)
			ss << "/";
		ss << "\">" << de->d_name;

		ss <<
		   "</th>"
		   "<th>";
		ss << Date(&sbuf.st_mtim).toGoodString();
		ss << "</th>"
		      "<th>";
		ss << sbuf.st_size;
		ss << "</th>"
		      "</tr>";

	}

	ss << "</table>"
	      "<address>Marisa/0.0.1</address>"
	      "</body></html>";

	response->send(std::move(ss.str()));
}
