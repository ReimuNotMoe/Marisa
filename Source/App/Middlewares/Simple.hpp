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
	class Simple : public Middleware {
	MARISA_MIDDLEWARE_USE_DEFAULT_CLONE
	public:
		std::shared_ptr<std::string> contents;

		explicit Simple(std::shared_ptr<std::string> __sptr) {
			contents = __sptr;
		}

		explicit Simple(std::string __contents) {
			contents = std::make_shared<std::string>(std::move(__contents));
		}

		void handler() override;


	};
}
