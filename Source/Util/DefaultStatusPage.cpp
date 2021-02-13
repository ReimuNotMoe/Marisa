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

using namespace Marisa;

static const char http_status_template[] = "<!DOCTYPE html>\n"
					   "<html>\n"
					   "<head><title>{status}</title></head>\n"
					   "<body bgcolor=\"white\">\n"
					   "<center><h1>{status}</h1></center>\n"
					   "<hr><center>Marisa/0.2</center>\n"
					   "</body>\n"
					   "</html>";

static std::unordered_map<int, std::string> status_code_strings = {
	{100, "Continue"},
	{101, "Switching Protocols"},
	{102, "Processing"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "Request-URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Requested Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Entity"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{444, "Connection Closed Without Response"},
	{451, "Unavailable For Legal Reasons"},
	{499, "Client Closed Request"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"},
	{599, "Network Connect Timeout Error"},
};

static std::unordered_map<int, std::string> cached_status_page;

static std::mutex lock;

const std::string& Util::default_status_page(int status) {
	std::lock_guard<std::mutex> lg(lock);

	auto it = cached_status_page.find(status);

	if (it == cached_status_page.end()) {
		std::string sbuf = std::to_string(status) + " " + status_code_strings[status];
		std::string page = fmt::format(http_status_template, fmt::arg("status", sbuf));
		cached_status_page[status] = std::move(page);
		return cached_status_page[status];
	} else {
		return it->second;
	}
}