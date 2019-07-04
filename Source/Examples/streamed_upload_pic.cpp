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

#include <iostream>
#include "../Marisa.hpp"

using namespace Marisa::Application;
using namespace Middlewares;


class upload_handler : public Middleware {
public:

	void handler() override {
		Mullet mp;
		mp.set_headers(request->headers_lowercase);

		Base64::Encoder b64_enc;

		std::string encoded;
		while (true) {
			auto buf = request->read(8192);
			std::cout << "read " << buf.size() << " bytes\n";
			mp.parse(buf);

			if (!mp.parts.empty()) {
				encoded += b64_enc.encode(mp.parts[0].data);
				std::cout << "encoded size = " << encoded.size() << "\n";
				mp.parts[0].data.clear();
			}

			if (mp.done()) { // Since we only expect one part here, we can just check if we have processed all parts
				std::cout << "done\n";
				encoded += b64_enc.finalize();
				std::cout << "done, encoded size = " << encoded.size() << "\n";
				break;
			}
		}

		try {
			response->write("<html><head>"
				       "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
				       "<title>Streamed picture upload test</title>"
				       "</head>"
				       "<body>"
				       "<h1>Streamed picture upload test</h1>"
				       "<br><br>"
				       "<form action=\"/post\" method=\"post\" enctype=\"multipart/form-data\">"
				       "<input type=\"file\" name=\"myfile\"/>"
				       "<input type=\"submit\" value=\"Upload\"/>"
				       "</form>"
				       "<img src=\"data:image/png;base64,");

			response->write(encoded);

			response->write("\"></img></body>"
				       "</html>");
			response->end();
		} catch (...) {

		}
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<upload_handler>();
	}
};


int main() {
	App myapp;

	myapp.route("/post*").stream().on("POST").use(upload_handler());

	myapp.route("/").on("GET").use(Simple("<html><head>"
					      "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
					      "<title>Streamed picture upload test</title>"
					      "</head>"
					      "<body>"
					      "<h1>Streamed picture upload test</h1>"
					      "<br><br>"
					      "<form action=\"/post\" method=\"post\" enctype=\"multipart/form-data\">"
					      "<input type=\"file\" name=\"myfile\"/>"
					      "<input type=\"submit\" value=\"Upload\"/>"
					      "</form>"
					      "</body>"
					      "</html>"));

	myapp.listen(8080);
	myapp.run();
}
