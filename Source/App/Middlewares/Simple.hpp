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
	class Simple : public Middleware {
	public:
		std::shared_ptr<std::string> contents;

		explicit Simple(std::shared_ptr<std::string> __sptr) {
			contents = __sptr;
		}

		explicit Simple(std::string __contents) {
			contents = std::make_shared<std::string>(std::move(__contents));
		}

		void handler() override;

		std::unique_ptr<Middleware> New() const override {
			return std::make_unique<Simple>(contents);
		}

	};
}
