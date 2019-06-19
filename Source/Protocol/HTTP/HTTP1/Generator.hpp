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
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MARISA_PROTOCOL_HTTP1_GENERATOR_HPP
#define MARISA_PROTOCOL_HTTP1_GENERATOR_HPP

#include "../Generator.hpp"

namespace Marisa {
	namespace Protocol {
		namespace HTTP1 {

			class Generator : public HTTP::Generator {
			protected:
			public:
				Generator() = default;

				std::string generate_headers(ResponseContext& resp_ctx) override;
				std::string generate_all(ResponseContext& resp_ctx) override;
			};
		}

	}
}

#endif //MARISA_PROTOCOL_HTTP1_GENERATOR_HPP
