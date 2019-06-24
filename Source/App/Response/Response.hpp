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

#ifndef MARISA_APP_RESPONSE_HPP
#define MARISA_APP_RESPONSE_HPP

#include "../../CommonIncludes.hpp"
#include "Body.hpp"
#include "../../Types/Date.hpp"
#include "../../Protocol/HTTP/Status.hpp"
#include "../../Log/Log.hpp"
#include "../../Server/Buffer.hpp"

using namespace Marisa::Types;
using namespace Marisa::Protocol;
using namespace Marisa::Log;
using namespace Marisa::Server;

namespace Marisa {
	namespace Server {
		class Session;
	}

	namespace Application {
		class App;
		class AppExposed;
		class Route;
		class Context;

		namespace Response {
			class Body;

			class ResponseContext {
			protected:
				Buffer buf_write;
				bool headers_sent = false;

				virtual void do_write(bool __blocking);
				virtual void do_raw_write(bool __blocking);

			public:
				std::unordered_map<std::string, std::string> headers;
				HTTP::Status status;
				Date date;
				ssize_t length = -1;
				Context *context = nullptr;
				Session *session = nullptr;
				boost::asio::yield_context *yield_context = nullptr;


				virtual void raw_write(std::string __s, bool __blocking = false);
				virtual void raw_write(std::vector<uint8_t> __s, bool __blocking = false);
				virtual void raw_write(const void *__s, size_t __len, bool __blocking = false);

				virtual void write(std::string __s, bool __blocking = false);
				virtual void write(std::vector<uint8_t> __s, bool __blocking = false);
				virtual void write(const void *__s, size_t __len, bool __blocking = false);

				virtual void end();

				virtual void send(std::string __s, bool __blocking = false);
				virtual void send(std::vector<uint8_t> __s, bool __blocking = false);
				virtual void send(const void *__s, size_t __len, bool __blocking = false);

				virtual void send_headers(uint16_t flags = 0);
				virtual void send_file(const std::string& __file_path, size_t __buffer_size = 16384);

				std::string type = "text/html; charset=utf-8";

				explicit ResponseContext(Context *__context);
				virtual ~ResponseContext();

			};

			class ResponseContextExposed : public ResponseContext {
			public:
//				using ResponseContext::Generate;

				explicit ResponseContextExposed(Context *__context) : ResponseContext::ResponseContext(__context) {};
			};
		}
	}
}

#endif //MARISA_APP_RESPONSE_HPP
