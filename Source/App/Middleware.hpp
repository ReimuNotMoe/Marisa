/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../CommonIncludes.hpp"

#include "Context.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define MARISA_MIDDLEWARE_USE_DEFAULT_CLONE		public: std::unique_ptr<Middleware> clone() const override {return std::make_unique<std::decay_t<decltype(*this)>>(*this);}

namespace Marisa {
	class Context;

	class Middleware {
	public:
		Context *context = nullptr;
		Request *request = nullptr;
		Response *response = nullptr;

		void __load_context(Context *__context) noexcept;

		void next();

		virtual void handler() = 0;

		virtual std::unique_ptr<Middleware> clone() const = 0;

		virtual ~Middleware() = default;
	};

}