/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "StaticFiles.hpp"

static const char ModuleName[] = "StaticFiles";

using namespace Marisa;
using namespace Middlewares;
using namespace Util;

void StaticFiles::handler() {
	auto it_path = request->url_vars().find("1");

	if (it_path == request->url_vars().end()) {
		context->logger->error("[{} @ {:x}] Incorrectly configured route. Please use something like `/foo/bar/**'.", ModuleName, (intptr_t)this);
		response->status = 500;
		response->send(default_status_page(response->status));
		return;
	}

	auto path = decodeURIComponent(it_path->second);

	// Get rid of script kiddies
	if (path.find("/../") != std::string::npos) {
		response->status = 200;
		response->send("You are a teapot");
		return;
	}

	auto full_path = base_path + "/" + path;

	context->logger->debug("[{} @ {:x}] full_path: {}", ModuleName, (intptr_t)this, full_path);


	struct stat stat_buf;
	if (stat(full_path.c_str(), &stat_buf)) {
		if (errno == ENOENT) {
			response->status = 404;
		} else if (errno == EPERM) {
			response->status = 403;
		} else {
			response->status = 500;
		}

		response->send(default_status_page(response->status));
		return;
	}

	DIR *dirp = nullptr;

	if ((dirp = opendir((full_path + "/").c_str()))) { // Directory
		if (list_files) {
			generate_file_page(dirp);
		} else {
			response->status = 403;
			response->send(default_status_page(response->status));
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
					response->header["Content-Type"] = "application/octet-stream";
				else
					response->header["Content-Type"] = mime_type(path.substr(dot_pos + 1));

				response->send_file(full_path);
				return;
			} catch (std::system_error& e) {
				context->logger->error("[{} @ {:x}] send_file error: {}", ModuleName, (intptr_t)this, e.what());
				response->status = 500;
				response->send(default_status_page(response->status));
				return;
			}
		} else {
			response->status = 500;
		}
		response->send(default_status_page(response->status));
		return;
	}
}

void StaticFiles::generate_file_page(DIR *__dirp) {
	errno = 0;

	std::stringstream ss;

	std::string title = "Index of " + request->url();

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

		timespec mtim_;

#ifdef __APPLE__
		mtim_ = sbuf.st_mtimespec;
#else
		mtim_ = sbuf.st_mtim;
#endif

		ss << JSDate(&mtim_).toGoodString();
		ss << "</th>"
		      "<th>";
		ss << sbuf.st_size;
		ss << "</th>"
		      "</tr>";

	}

	ss << "</table>"
	      "<hr width=\"100%\"><address>Marisa/" MARISA_VERSION "</address>"
	      "</body></html>";

	response->send(std::move(ss.str()));
}
