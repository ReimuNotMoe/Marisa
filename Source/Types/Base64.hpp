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

#ifndef MARISA_TYPE_BASE64_HPP
#define MARISA_TYPE_BASE64_HPP

#include "../CommonIncludes.hpp"
#include "../3rdParty/base64/include/libbase64.h"

namespace Marisa {
	namespace Types {
		namespace Base64 {
			class Encoder {
			protected:
				base64_state b64_state;
				size_t encoded_size;
			public:
				Encoder();

				void reset();

				std::string encode(std::string_view __input);
				std::string finalize();

				static std::string encode_once(std::string_view __input);
			};

			class Decoder {
			protected:
				base64_state b64_state;
				size_t decoded_size;
			public:
				Decoder();

				void reset();
				std::string decode(std::string_view __input);

				static std::string decode_once(std::string_view __input);
			};
		}
	}
}
#endif //MARISA_TYPE_BASE64_HPP
