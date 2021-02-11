/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <iostream>
#include "../Marisa.hpp"

using namespace Marisa;
using namespace Marisa::Middlewares;

const char html_part1[] = "<html><head>"
			  "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
			  "<title>Streamed picture upload test</title>"
			  "</head>"
			  "<body>"
			  "<h1>Streamed picture upload test</h1>"
			  "<br><br>"
			  "<form action=\"/\" method=\"post\" enctype=\"multipart/form-data\">"
			  "<input type=\"file\" name=\"myfile\"/>"
			  "<input type=\"submit\" value=\"Upload\"/>"
			  "</form>";
const char html_part2[] = "<img src=\"data:image/png;base64,";
const char html_part3[] = "\"></img>";
const char html_part4[] = "</body></html>";

int main() {
	App myapp;

	myapp.route("/").stream().use([](Request *req, Response *rsp, auto *ctx) {
		if (req->method() == "GET") {
			rsp->write(html_part1);
			rsp->write(html_part4);
			rsp->end();
		} else if (req->method() == "POST") {
			std::string encoded;
			Base64X::Encoder<std::string> b64_enc;

			while (req->read_post([&](auto &key, const std::string_view &value, auto &filename, auto &content_type, auto &transfer_encoding) {
				if (!value.empty()) {
					encoded += b64_enc.encode(value);
//					std::cout << "encoded size = " << encoded.size() << "\n";
				}
			}));

			encoded += b64_enc.finalize();
			std::cout << "done, encoded size = " << encoded.size() << "\n";

			rsp->write(html_part1);
			rsp->write(html_part2);
			rsp->write(encoded);
			rsp->write(html_part3);
			rsp->write(html_part4);
			rsp->end();
		}
	});

	myapp.listen(8080);
	myapp.start(2);

	while (1)
		sleep(-1);
}
