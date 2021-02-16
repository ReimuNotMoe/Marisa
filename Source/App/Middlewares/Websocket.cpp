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

using namespace Marisa;

std::function<void(Request *, Response *, Context *)> Middlewares::Websocket() {
	return [](Request *request, Response *response, Context *context) {
		auto ws_ver = request->header("Sec-WebSocket-Version");
		auto ws_key = request->header("Sec-WebSocket-Key");

		if (request->method() != "GET" || ws_ver.empty() || ws_key.empty()) {
			response->status = 400;
			response->end();
		} else {
			response->upgrade();
		}
	};
}
