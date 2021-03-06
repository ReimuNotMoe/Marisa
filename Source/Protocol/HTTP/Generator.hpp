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
#include "Status.hpp"
#include "Method.hpp"

#include "../../App/Response/Response.hpp"

using namespace Marisa::Application::Response;

namespace Marisa {
	namespace Protocol {
		namespace HTTP {

			class Generator {
			protected:

			public:
				Generator() = default;

				int keepalive_config[2] = {-1};

				virtual std::vector<uint8_t> generate_all(ResponseContext& resp_ctx) = 0;
				virtual std::vector<uint8_t> generate_headers(ResponseContext& resp_ctx) = 0;

			};
		}
	}
}