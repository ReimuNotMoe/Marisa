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

std::function<void(Request *, Response *, Context *)> Middlewares::Simple(std::string str) {
	return [str_ = std::make_shared<std::string>(std::move(str))](Request *request, Response *response, Context *context){
		response->send_persistent(str_->data(), str_->size());
	};
}