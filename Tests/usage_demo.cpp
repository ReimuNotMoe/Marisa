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

#include <sstream>
#include <iostream>

using namespace Marisa::Application;
using namespace Middlewares;


class usage_demo : public Middleware {
public:
	std::string str;

	explicit usage_demo(std::string __str) {
		str = std::move(__str);
	}

	void handler() override {
		std::ostringstream ss;
		ss << "<html><head>\n"
		   << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
		   << "<title>" << str << "</title>\n"
		   << "</head>";



		ss << "<body>"
		   << "<h1>Hello world!</h1>"
		   << "<hr>"
		   << "URL: " << request->url << "<br>"
		   << "Method: " << request->method.to_string() << "<br>"
		   << "Your IP address: " << request->ip() << "<br>"
		   << "Your socket IP address: " << request->socket.remote_endpoint().address().to_string() << "<br>"
		   << "Your port: " << request->socket.remote_endpoint().port() << "<br><br>";


		ss  << "Headers:<br>"
		    << "<table style=\"width:100%\" border=\"1\">"
		       "<tr>"
		       "<th>Key</th>"
		       "<th>Value</th>"
		       "</tr>";


		for (auto &it : request->headers) {
			ss << "<tr>";
			ss << "<td>" << it.first << "</td>";
			ss << "<td>" << it.second << "</td>";
			ss << "</tr>";
		}


		ss << "</table>";

		ss << "<br><br>";

		ss  << "Cookies:<br>"
		    << "<table style=\"width:100%\" border=\"3\">"
		       "<tr>"
		       "<th>Key</th>"
		       "<th>Value</th>"
		       "</tr>";


		for (auto &it : request->cookies()) {
			ss << "<tr>";
			ss << "<td>" << it.first << "</td>";
			ss << "<td>" << it.second << "</td>";
			ss << "</tr>";
		}


		ss << "</table>";

		ss << "<br><br>";

		ss  << "URL smatches:<br>"
		    << "<table style=\"width:100%\" border=\"3\">"
		       "<tr>"
		       "<th>Key</th>"
		       "<th>Value</th>"
		       "</tr>";


		size_t i=0;
		for (auto &it : request->url_smatch) {
			ss << "<tr>";
			ss << "<td>" << "url_smatch[" << i << "]" << "</td>";
			ss << "<td>" << it << "</td>";
			ss << "</tr>";
			i++;
		}


		ss << "</table>";

		ss << "<br><br>";

		ss << "URI encode/decode tests:" << "<br>";
		ss << "<code>";
		ss << "encodeURI(\"aa=bb&amp;cc=dd大花猫+++\") = \"" << encodeURI("aa=bb&cc=dd大花猫+++") << "\"<br>";
		ss << "encodeURIComponent(\"aa=bb&amp;cc=dd大花猫+++\") = \"" << encodeURIComponent("aa=bb&cc=dd大花猫+++") << "\"<br>";
		ss << R"(decodeURI("aa=bb&amp;cc=dd%E5%A4%A7%E8%8A%B1%E7%8C%AB+++") = ")" << decodeURI("aa=bb&cc=dd%E5%A4%A7%E8%8A%B1%E7%8C%AB+++") << "\"<br>";
		ss << R"(decodeURIComponent("aa%3Dbb%26cc%3Ddd%E5%A4%A7%E8%8A%B1%E7%8C%AB%2B%2B%2B") = ")" << decodeURIComponent("aa%3Dbb%26cc%3Ddd%E5%A4%A7%E8%8A%B1%E7%8C%AB%2B%2B%2B") << "\"<br>";
		ss << R"(decodeURI("aa%3Dbb%26cc%3Ddd%E5%A4%A7%E8%8A%B1%E7%8C%AB%2B%2B%2B") = ")" << decodeURI("aa%3Dbb%26cc%3Ddd%E5%A4%A7%E8%8A%B1%E7%8C%AB%2B%2B%2B") << "\"<br>";
		ss << R"(decodeURIComponent("aa=bb&amp;cc=dd%E5%A4%A7%E8%8A%B1%E7%8C%AB+++") = ")" << decodeURIComponent("aa=bb&cc=dd%E5%A4%A7%E8%8A%B1%E7%8C%AB+++") << "\"<br>";
		ss << "</code>";

		ss << "<br><br>";

		ss << "base64 encode/decode tests:" << "<br>";
		ss << "<code>";
		ss << "Base64::Encoder::encode_once(\"The quick brown fox jumps over the lazy dog\") = \"" << Base64::Encoder::encode_once("The quick brown fox jumps over the lazy dog") << "\"<br>";
		ss << "Base64::Decoder::decode_once(\"VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw\") = \"" << Base64::Decoder::decode_once("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw") << "\"<br>";
		ss << "</code>";

		ss << "<hr>Marisa/" << Marisa::Version << "\n";
		ss << "</body></html>";

		response->send(std::move(ss.str()));
//		response->end();
	}


	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<usage_demo>(str);
	}

};


int main() {
	App myapp;

	myapp.route("*").on("GET").use(Compression()).use(usage_demo("Usage demo"));

	auto &route = myapp.route("/*");
	route.on("GET").use(Compression()).use(usage_demo("Usage dem0"));

	myapp.listen(8080);
	myapp.run();
}