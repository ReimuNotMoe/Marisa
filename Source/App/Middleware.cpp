/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Middleware.hpp"
#include "Context.hpp"

using namespace Marisa;

void Middleware::__load_context(Context *__context) noexcept {
	context = __context;
	request = &context->request;
	response = &context->response;
}

void Middleware::next() {
	context->next();
}
