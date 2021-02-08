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

#include "CommonIncludes.hpp"

namespace Marisa::Middlewares {
	class Redirection : public Middleware {
	MARISA_MIDDLEWARE_USE_DEFAULT_CLONE
	public:
		std::string location;
		int status_code;

		explicit Redirection(std::string __location, int __status_code = 302) {
			location = std::move(__location);
			status_code = __status_code;
		}

		void handler() override;
	};
}
