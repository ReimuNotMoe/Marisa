/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Middlewares.hpp"

#include <sys/types.h>
#include <dirent.h>

static const char ModuleName[] = "StaticFiles";

using namespace Marisa;
using namespace Marisa::Util;

static void generate_file_page(const std::string& base_path, DIR *__dirp, Request *request, Response *response, Context *context) {
	errno = 0;

	std::stringstream ss;

	std::string title = "Index of " + request->url();

	ss << "<!DOCTYPE HTML>"
	      "<html>"
	      "<head>"
	      "<title>"
	   << title
	   << "</title>"
	      "<style>\n"
	      " html, body {\n"
	      "  font-family: Monospace;"
	      "  overflow: hidden;\n"
	      "  overflow-y: auto;\n"
	      " }\n"
	      "\n"
	      " table {\n"
	      "  width: 100%;\n"
	      "  border: none;\n"
	      "  border-collapse: collapse;\n"
	      " }\n"
	      "\n"
	      " tr:nth-child(2n+1) {\n"
	      "  border-width: 0;\n"
	      "  border-style: none;\n"
	      "  padding: 0;\n"
	      "  background-color: gainsboro;\n"
	      " }\n"
	      "\n"
	      " tr>th:nth-child(1), tr>td:nth-child(1) {\n"
	      "  background-color: #0000001f;\n"
	      "  text-align: center;\n"
	      "  width: 5%;\n"
	      "  min-width: 20px;\n"
	      "  max-width: 40px;\n"
	      " }\n"
	      "\n"
	      " th, td {\n"
	      "  border: none;\n"
	      "  word-break: break-word;\n"
	      "  padding: 4px 12px;\n"
	      "  text-align: left;\n"
	      " }\n"
	      "\n"
	      " code {\n"
	      "  word-break: break-word;\n"
	      " }\n"
	      "</style>"
	      "</head>"
	      "<body>"
	      "<h1>"
	   << title
	   << "</h1>"
	      "<table>"
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
		      "<th>";

		if (de->d_type == DT_DIR)
			ss << "DIR";
		else if (de->d_type == DT_REG)
			ss << "REG";
		else if (de->d_type == DT_LNK)
			ss << "LNK";
		else if (de->d_type == DT_SOCK)
			ss << "SOCK";

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

		ss << "TODO";
		ss << "</th>"
		      "<th>";
		ss << sbuf.st_size;
		ss << "</th>"
		      "</tr>";

	}

	ss << "</table>"
	      "<hr width=\"100%\"><address>Marisa/" MARISA_VERSION "</address>"
	      "</body></html>";

	response->send(ss.str());
}

std::function<void(Request *, Response *, Context *)> Middlewares::StaticFiles(std::string base_path, bool list_files) {
	return [ctx = std::make_shared<std::pair<std::string, bool>>(std::move(base_path), list_files)](Request *request, Response *response, Context *context){
		auto &base_path = ctx->first;
		auto &list_files = ctx->second;

		auto it_path = request->url_vars().find("1");

		if (it_path == request->url_vars().end()) {
			context->logger->error("[{} @ ?] Incorrectly configured route. Please use something like `/foo/bar/**'.", ModuleName);
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

		context->logger->debug("[{} @ ?] full_path: {}", ModuleName, full_path);


		struct stat stat_buf;
		if (stat(full_path.c_str(), &stat_buf)) {
			if (errno == ENOENT) {
				response->status = 404;
			} else if (errno == EPERM) {
				response->status = 403;
			} else {
				response->status = 500;
			}

			response->send_status_page();
			return;
		}

		DIR *dirp = nullptr;

		if ((dirp = opendir((full_path + "/").c_str()))) { // Directory
			if (list_files) {
				generate_file_page(base_path, dirp, request, response, context);
			} else {
				response->status = 403;
				response->send_status_page();
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
					context->logger->error("[{} @ ?] send_file error: {}", ModuleName, e.what());
					response->status = 500;
					response->send_status_page();
					return;
				}
			} else {
				response->status = 500;
			}
			response->send_status_page();
			return;
		}
	};
}
