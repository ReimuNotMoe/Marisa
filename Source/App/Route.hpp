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

namespace Marisa {
	class Middleware;
	class Request;
	class Response;
	class Context;

	class Route {
	protected:
		std::optional<std::vector<std::string>> path_keys;
		std::vector<std::function<void(Request *, Response *, Context *)>> middlewares;
		bool mode_streamed = false;
		bool mode_async = false;

	public:
		Route() = default;

		Route &stream();
		Route &async();

		Route& use(std::function<void(Request *, Response *, Context *)> func);
	};

	class RouteExposed : public Route {
	public:
		using Route::path_keys;
		using Route::middlewares;
		using Route::mode_streamed;
		using Route::mode_async;

	};

}
