/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <Marisa.hpp>

using namespace Marisa;
using namespace Middlewares;

int main() {
	App myapp;

	myapp.route("/").stream().use([](auto *req, auto *rsp, auto *ctx){
		rsp->write("<html><head>"
			  "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
			  "<title>Hello Marisa!</title>"
			  "</head>"
			  "<body>");

		rsp->write("<h1>Hello Marisa!</h1>"
			  "</body>"
			  "</html>");

		rsp->end();
	});

	myapp.listen(8080);
	myapp.start();

	while (1) {
		sleep(-1);
	}
}