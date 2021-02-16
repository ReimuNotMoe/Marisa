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

int main() {
	App myapp;

	myapp.route("/").async().use([](auto *req, auto *rsp, Context *ctx){
		rsp->send("");
	});

	myapp.route("/user/:id").async().use([](Request *req, auto *rsp, auto *ctx){
		rsp->send(req->url_vars()["id"]);
	});

	myapp.route("/user").async().use([](auto *req, Response *rsp, auto *ctx){
		rsp->send("");
	});

	myapp.listen(3000);
	myapp.start();

	while (1) {
		sleep(-1);
	}
}