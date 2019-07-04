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

#include "../Marisa.hpp"

using namespace Marisa::Application;
using namespace Middlewares;

int main() {
	App myapp;

	myapp.route("/").on("GET").use(Simple(
		"<html><head>"
		"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
		"<title>Hello Marisa!</title>"
		"</head>"
		"<body>"
		"<h1>Hello Marisa!</h1>"
		"</body>"
		"</html>"
	));

	myapp.listen_ssl(8443, [](auto& ssl_ctx){
		ssl_ctx.use_certificate_file("/etc/ssl/certs/ssl-cert-snakeoil.pem", boost::asio::ssl::context_base::file_format::pem);
		ssl_ctx.use_private_key_file("/etc/ssl/private/ssl-cert-snakeoil.key", boost::asio::ssl::context_base::file_format::pem);
	});


	myapp.run();
}