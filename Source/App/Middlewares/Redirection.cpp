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

std::function<void(Request *, Response *, Context *)> Middlewares::Redirection(std::string location, int status_code) {
	return [ctx = std::make_shared<std::pair<std::string, int>>(std::move(location), status_code)](Request *request, Response *response, Context *context){
		response->header["Location"] = ctx->first;
		response->status = ctx->second;
		response->end();
	};
}