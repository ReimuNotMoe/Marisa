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

#ifndef MARISA_APPMW_LAMBDA_HPP
#define MARISA_APPMW_LAMBDA_HPP

#include "CommonIncludes.hpp"

namespace Marisa::Application::Middlewares {
	class Lambda : public Middleware {
	public:
		void (*func)(Request::RequestContext *, Response::ResponseContext *, Context *) = nullptr;

		explicit Lambda(void (*__func)(Request::RequestContext *, Response::ResponseContext *, Context *)) {
			func = __func;
		}

		void handler() override {
			func(request, response, context);
		}

		std::unique_ptr<Middleware> New() const override {
			return std::make_unique<Lambda>(func);
		}

	};
}

#endif //MARISA_APPMW_LAMBDA_HPP
