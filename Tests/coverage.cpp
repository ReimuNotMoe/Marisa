/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

// TODO

#include <Marisa.hpp>

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

		ss << "<hr>Marisa/" << MARISA_VERSION << "\n";
		ss << "</body></html>";

		response->send(ss.str());
	}
};

int main() {

	JSDate d0("Fri, 05 Jul 2019 07:47:57 GMT");
	JSDate dNow;

	std::cout << dNow.toString() << "\n";
	std::cout << dNow.toDateString() << "\n";
	std::cout << dNow.toGMTString() << "\n";
	std::cout << dNow.toGoodString() << "\n";
	std::cout << dNow.toISOString() << "\n";

	JSDate d1;

	d1.assign_jstime(0);
	d1.assign_unixtime(0);

	timespec ts1;
	clock_gettime(CLOCK_REALTIME, &ts1);
	d1.assign_timespec(&ts1);

	const char static_arr[] = "Hello Marisa";
	std::vector<uint8_t> vec;
	vec.insert(vec.end(), static_arr, static_arr+sizeof(static_arr)-1);
	App myapp;

	myapp.route("/usage_demo").use(usage_demo(""));
	myapp.route("/simple").use(Simple("Hello Marisa"));
	myapp.route("/lambda").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
	}));

	myapp.route("/redirect_301").use(Redirection("/simple", 301));
	myapp.route("/redirect_302").use(Redirection("/simple", 302));

	myapp.route("/error_catcher").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
		throw std::invalid_argument("what's up doc");
	}));


	myapp.route("/sendfile").use([](auto req, auto rsp, auto ctx){
		rsp->send_file("/etc/profile");
	});

	myapp.route("/static_files/*").use(StaticFiles(".", true));


	myapp.route("/streamed_fileupload").stream().use([](Request *req, Response *rsp, auto *ctx) {
		if (req->method() == "GET") {
			rsp->end();
		} else if (req->method() == "POST") {
			std::string encoded;
			Base64X::Encoder<std::string> b64_enc;

			while (req->read_post([&](auto &key, const std::string_view &value, auto &filename, auto &content_type, auto &transfer_encoding) {
				if (!value.empty()) {
					encoded += b64_enc.encode(value);
				}
			}));

			encoded += b64_enc.finalize();
			std::cout << "done, encoded size = " << encoded.size() << "\n";

			rsp->write(encoded);
			rsp->end();
		}
	});

	myapp.route(std::regex("^whatever")).use(Simple("whatever!!"));


	myapp.listen(46878);
	myapp.start();

	sleep(2);

	std::string cmd_get_prefix = "curl -v -H 'Cookie: aaa=bbb;ccc=ddd' 127.0.0.1:46878/";

	for (auto &url : {"usage_demo", "simple", "lambda",
			  "redirect_301", "redirect_302", "error_catcher",
			  "sendfile", "static_files/", "streamed_fileupload",
			  "whateveraaa", "whateverbbb", "404"}) {
		auto t = cmd_get_prefix + url;
		system(t.c_str());
	}

//	system(R"(curl -v -X POST -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" 127.0.0.1:46878/async_lambda_post)");
//
//	system("curl -v -H 'Accept-Encoding: gzip, deflate' 127.0.0.1:46878/noyield_lambda_send_compressed");
//	system("curl -v -H 'Accept-Encoding: deflate, gzip' 127.0.0.1:46878/noyield_lambda_send_compressed");
//	system("curl -v -H 'Accept-Encoding: deflate' 127.0.0.1:46878/noyield_lambda_send_compressed");
//	system("curl -v -H 'Accept-Encoding: gzip' 127.0.0.1:46878/noyield_lambda_send_compressed");
//
//	system("curl -v -X POST -F 'data=@CMakeCache.txt' 127.0.0.1:46878/blocking_streamed_fileupload");
//	system("curl -v -X POST -F 'data=@CMakeCache.txt' 127.0.0.1:46878/async_streamed_fileupload");
//
//	system("nc -nv -w 20 127.0.0.1 46878");

	myapp.stop();

	return 0;
}