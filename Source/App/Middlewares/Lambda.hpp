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

#pragma once

#include "CommonIncludes.hpp"

namespace Marisa::Application::Middlewares {
	class Lambda : public Middleware {
	public:

		std::function<void(Request::RequestContext *, Response::ResponseContext *, Context *)> func;
//		void (*func)(Request::RequestContext *, Response::ResponseContext *, Context *) = nullptr;

		explicit Lambda(std::function<void(Request::RequestContext *, Response::ResponseContext *, Context *)> __func) {
			func = std::move(__func);
		}

		void handler() override {
			func(request, response, context);
		}

		std::unique_ptr<Middleware> New() const override {
			return std::make_unique<Lambda>(func);
		}

	};
}
