/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Route.hpp"
#include "Middlewares/Lambda.hpp"

using namespace Marisa;

Route &Route::stream() {
	mode_streamed = true;
	return *this;
}

Route &Route::nonblocking() {
	mode_nonblocking = true;
	return *this;
}

Route &Route::use(const Middleware &middleware) {
	middlewares.emplace_back(middleware.clone());
	return *this;
}

Route &Route::use(std::function<void(Request *, Response *, Context *)> func) {
	middlewares.emplace_back(std::make_unique<Middlewares::Lambda>(std::move(func)));
	return *this;
}


