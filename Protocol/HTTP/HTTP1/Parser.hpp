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

#ifndef MARISA_PROTOCOL_HTTP1_PARSER_HPP
#define MARISA_PROTOCOL_HTTP1_PARSER_HPP

#include "../Parser.hpp"

namespace Marisa {
	namespace Protocol {
		namespace HTTP1 {
			class Parser : public Protocol::HTTP::Parser {
			protected:
				http_parser hp_ctx{};
				http_parser_settings hp_settings{};

				static int callback_status(http_parser *hp_ctx, const char *at, size_t length);
				static int callback_url(http_parser *hp_ctx, const char *at, size_t length);
				static int callback_header_field(http_parser *hp_ctx, const char *at, size_t length);
				static int callback_header_value(http_parser *hp_ctx, const char *at, size_t length);
				static int callback_body(http_parser *hp_ctx, const char *at, size_t length);
				static int callback_message_complete(http_parser *hp_ctx);
				static int callback_headers_complete(http_parser *hp_ctx);

				void parse_url();

				bool __finished = false;
				bool __header_finished = false;

				http_parser_type type = (http_parser_type) 0;

				http_data_cb last_data_cb = nullptr;
				std::string last_field, last_value;



			public:
				Parser();

				void init();

				ssize_t parse(const char *data, size_t len) override;


				bool finished() const noexcept override;
				bool headers_finished() const noexcept override;



			};
		}
	}
}

#endif //MARISA_PARSER_HPP
