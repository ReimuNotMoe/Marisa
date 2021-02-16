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

using namespace Marisa;

Route &Route::stream() {
	mode_streamed = true;
	return *this;
}

Route &Route::async() {
	mode_async = true;
	return *this;
}

Route &Route::use(std::function<void(Request *, Response *, Context *)> func) {
	middlewares.emplace_back(std::move(func));
	return *this;
}
