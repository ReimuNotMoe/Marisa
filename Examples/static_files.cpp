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

	myapp.route("/files/**").use(StaticFiles("/tmp", true));

//	myapp.route("/files").on("*").use(Redirection("/files/", 301));

	myapp.listen(8080);
	myapp.start();

	while (1) {
		sleep(-1);
	}
}