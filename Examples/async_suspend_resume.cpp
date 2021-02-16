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

	myapp.route("/").async().use([state = 0](auto *req, Response *rsp, Context *ctx) mutable {
		if (!state) {
			std::thread ([&, ctx]{
				// Do some blocking stuff here
				sleep(3);
				state = 1;
				ctx->resume();
			}).detach();

			ctx->suspend();
			puts("suspend!");
		} else {
			rsp->send("I'm back!");
		}
	});

	myapp.listen(8080);
	myapp.start();

	while (1) {
		sleep(-1);
	}
}