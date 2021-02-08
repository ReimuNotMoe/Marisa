/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "../Marisa.hpp"

#include <iostream>

using namespace Marisa;
using namespace Middlewares;
using namespace Util;

class usage_demo : public Middleware {
MARISA_MIDDLEWARE_USE_DEFAULT_CLONE
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
		   << "URL: " << request->url() << "<br>"
		   << "Method: " << request->method() << "<br>"
		   << "Socket address: " << request->socket_address().to_string() << "<br><br>";


		ss  << "Headers:<br>"
		    << "<table style=\"width:100%\" border=\"1\">"
		       "<tr>"
		       "<th>Key</th>"
		       "<th>Value</th>"
		       "</tr>";


		for (auto &it : request->header()) {
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


		for (auto &it : request->cookie()) {
			ss << "<tr>";
			ss << "<td>" << it.first << "</td>";
			ss << "<td>" << it.second << "</td>";
			ss << "</tr>";
		}


		ss << "</table>";

		ss << "<br><br>";

		ss  << "URL matched capture groups:<br>"
		    << "<table style=\"width:100%\" border=\"3\">"
		       "<tr>"
		       "<th>Index</th>"
		       "<th>Value</th>"
		       "</tr>";


		size_t i=0;
		for (auto &it : request->url_matched_capture_groups()) {
			ss << "<tr>";
			ss << "<td>" << i << "</td>";
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

//		ss << "base64 encode/decode tests:" << "<br>";
//		ss << "<code>";
//		ss << "Base64::Encoder::encode_once(\"The quick brown fox jumps over the lazy dog\") = \"" << Base64::Encoder::encode_once("The quick brown fox jumps over the lazy dog") << "\"<br>";
//		ss << "Base64::Decoder::decode_once(\"VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw\") = \"" << Base64::Decoder::decode_once("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw") << "\"<br>";
//		ss << "</code>";

		ss << "<hr>Marisa/" << MARISA_VERSION << "\n";
		ss << "</body></html>";

		response->send(std::move(ss.str()));
//		response->end();
	}
};


int main() {
	App myapp;

	myapp.route("/**").use(usage_demo("Usage demo"));

	myapp.listen(8080);
	myapp.start();

	while (1) {
		sleep(-1);
	}
}