/*
    This file is part of Marisa.
    Copyright (C) 2018-2019 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Middleware.hpp"

using namespace Marisa::Application;

void Middleware::__load_context(ContextExposed *__context) noexcept {
	context = __context;
	request = context->request.get();
	response = context->response.get();
}

void Middleware::next() {
	context->next();
}

void Middleware::handler() {
	LogE("%s[0x%016" PRIxPTR "]:\toverrider of handler() hasn't been correctly defined\n", "Middleware", (uintptr_t)this);
	abort();
}

std::unique_ptr<Middleware> Middleware::New() const {
	LogE("%s[0x%016" PRIxPTR "]:\toverrider of New() hasn't been correctly defined\n", "Middleware", (uintptr_t)this);
	abort();
}

Middleware::~Middleware() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdestructor called\n", "Middleware", (uintptr_t)this);
#endif
}

void Middleware_RawIO::__load_context(ContextExposed *__context) noexcept {
	context = __context;
	session = context->session;
	yield_context = context->yield_context;
}


void Middleware_RawIO::handler() {
	LogE("%s[0x%016" PRIxPTR "]:\toverrider of handler() hasn't been correctly defined\n", "Middleware_RawIO", (uintptr_t)this);
	abort();
}

std::unique_ptr<Middleware_RawIO> Middleware_RawIO::New() const {
	LogE("%s[0x%016" PRIxPTR "]:\toverrider of New() hasn't been correctly defined\n", "Middleware_RawIO", (uintptr_t)this);
	abort();
}

Middleware_RawIO::~Middleware_RawIO() {
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tdestructor called\n", "Middleware_RawIO", (uintptr_t)this);
#endif
}



