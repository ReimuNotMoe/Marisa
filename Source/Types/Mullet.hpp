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

#include <unordered_map>
#include <deque>
#include <vector>

#include "../3rdParty/multipart-parser/MultipartParser.h"
#include "../3rdParty/multipart-parser/MultipartReader.h"

namespace Marisa {
	namespace Types {
		class Mullet {
		public:
			class Multipart_Part {
			public:
				std::unordered_map<std::string, std::string> headers;
				std::unordered_map<std::string, std::string_view> headers_lowercase;
				std::string data;
				bool done = false;
			};

		protected:
			bool __done = false;
			int last_handler = 0;

			std::string last_field, last_value;

			static void onPartBegin(const char *buffer, size_t start, size_t end, void *userData);
			static void onHeaderField(const char *buffer, size_t start, size_t end, void *userData);
			static void onHeaderValue(const char *buffer, size_t start, size_t end, void *userData);
			static void onPartData(const char *buffer, size_t start, size_t end, void *userData);
			static void onPartEnd(const char *buffer, size_t start, size_t end, void *userData);
			static void onEnd(const char *buffer, size_t start, size_t end, void *userData);

			std::string __boundary;
			::MultipartParser __parser;

		public:
			std::deque<Multipart_Part> parts;

			Mullet();
			explicit Mullet(const std::string& __data);

			void set_headers(const std::unordered_map<std::string, std::string_view>& __headers);
			void parse(const std::string_view& __data);
			void parse(const std::vector<uint8_t>& __data);
			bool done() const noexcept;


		};
	}
}