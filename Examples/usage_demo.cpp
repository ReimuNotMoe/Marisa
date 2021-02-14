/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <Marisa.hpp>

#include <iostream>
#include <ostream>

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
		response->measure_execution_time();

		auto t1 = std::chrono::high_resolution_clock::now();

		std::ostringstream ss;
		ss << "<html><head>\n"
		      "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
		   << "<title>" << str << "</title>\n"
					  "<style>\n"
					  " html, body {\n"
					  "  overflow: hidden;\n"
					  "  overflow-y: auto;\n"
					  " }\n"
					  "\n"
					  " table {\n"
					  "  width: 100%;\n"
					  "  border: none;\n"
					  "  border-collapse: collapse;\n"
					  " }\n"
					  "\n"
					  " tr:nth-child(2n+1) {\n"
					  "  border-width: 0;\n"
					  "  border-style: none;\n"
					  "  padding: 0;\n"
					  "  background-color: gainsboro;\n"
					  " }\n"
					  "\n"
					  " tr>th:nth-child(2n+1), tr>td:nth-child(2n+1) {\n"
					  "  background-color: #0000001f;\n"
					  "  width: 30%;\n"
					  " }\n"
					  "\n"
					  " th, td {\n"
					  "  border: none;\n"
					  "  word-break: break-word;\n"
					  "  padding: 4px 12px;\n"
					  " }\n"
					  "\n"
					  " code {\n"
					  "  word-break: break-word;\n"
					  " }\n"
					  "</style>"
					  "</head>";

		ss << "<body>"
		      "<h1>Hello world!</h1>"
		      "<hr>"
		      "URL: " << request->url() << "<br>"
						   "Method: " << request->method() << "<br>"
										      "Socket address: " << request->socket_address().to_string() << "<br><br>";


		ss  << "Headers:<br>"
		       "<table style=\"width:100%\" border=\"1\">"
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
		       "<table style=\"width:100%\" border=\"3\">"
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

		ss  << "URL Variables:<br>"
		    << "<table style=\"width:100%\" border=\"3\">"
		       "<tr>"
		       "<th>Key/Index</th>"
		       "<th>Value</th>"
		       "</tr>";


		for (auto &it : request->url_vars()) {
			ss << "<tr>";
			ss << "<td>" << it.first << "</td>";
			ss << "<td>" << it.second << "</td>";
			ss << "</tr>";
		}


		ss << "</table>";

		ss << "<br><br>";

		ss  << "URL Query Strings:<br>"
		       "<table style=\"width:100%\" border=\"3\">"
		       "<tr>"
		       "<th>Key</th>"
		       "<th>Value</th>"
		       "</tr>";


		for (auto &it : request->query()) {
			ss << "<tr>";
			ss << "<td>" << it.first << "</td>";
			ss << "<td>" << it.second << "</td>";
			ss << "</tr>";
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
		ss << "Page generated in: ";
		auto t2 = std::chrono::high_resolution_clock::now();

		auto duration_ms = (double)std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count() / 1000;
		ss << std::to_string(duration_ms);
		ss << " ms<br><br>";
//		ss << "base64 encode/decode tests:" << "<br>";
//		ss << "<code>";
//		ss << "Base64::Encoder::encode_once(\"The quick brown fox jumps over the lazy dog\") = \"" << Base64::Encoder::encode_once("The quick brown fox jumps over the lazy dog") << "\"<br>";
//		ss << "Base64::Decoder::decode_once(\"VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw\") = \"" << Base64::Decoder::decode_once("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw") << "\"<br>";
//		ss << "</code>";

		ss << "<hr>Marisa/" << MARISA_VERSION << "\n";
		ss << "</body></html>";

		response->send(ss.str());
//		response->end();
	}
};


int main() {
	App myapp;

	myapp.route("/:foo/:bar/**").use(usage_demo("Usage demo with path variables"));
	myapp.route("/**").use(usage_demo("Usage demo"));


	myapp.listen(8080);
	myapp.start();

	while (1) {
		sleep(-1);
	}
}