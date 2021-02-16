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

	myapp.route("/no_middlewares");

	myapp.route("/inline_run1").use([](auto req, auto rsp, Context *ctx){
		ctx->run(Simple("Hello Marisa"));
	});

	myapp.route("/inline_run2").use([](auto req, auto rsp, Context *ctx){
		ctx->run([](Request *req, auto rsp, Context *ctx){
			std::string a = "aaa";

			rsp->write(a);

			if (!req->header("Accept").empty()) {
				rsp->write("Has Accept header");
			}

			rsp->write("Header keys:");

			for (auto &it : req->header_keys()) {
				rsp->write(it);
				rsp->write("\n");
			}

			if (!req->query("aaa").empty()) {
				rsp->write("Has aaa query");
			}

			rsp->write("Query keys:");

			for (auto &it : req->query_keys()) {
				rsp->write(it);
				rsp->write("\n");
			}

			if (!req->cookie("aaa").empty()) {
				rsp->write("Has aaa cookie");
			}

			rsp->write("Cookie keys:");

			for (auto &it : req->cookie_keys()) {
				rsp->write(it);
				rsp->write("\n");
			}

			if (!req->post("aaa").key.empty()) {
				rsp->write("Has aaa post");
			}

			rsp->write("Post keys:");

			for (auto &it : req->post_keys()) {
				rsp->write(it);
				rsp->write("\n");
			}

			rsp->write("Hello Marisa");
		});
	});

	auto func = [](Request *request, Response *response, Context *context){
		response->measure_execution_time();

		std::ostringstream ss;
		ss << "<html><head>\n"
		      "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
		   << "<title>Usage Demo</title>\n"
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

//		ss << "base64 encode/decode tests:" << "<br>";
//		ss << "<code>";
//		ss << "Base64::Encoder::encode_once(\"The quick brown fox jumps over the lazy dog\") = \"" << Base64::Encoder::encode_once("The quick brown fox jumps over the lazy dog") << "\"<br>";
//		ss << "Base64::Decoder::decode_once(\"VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw\") = \"" << Base64::Decoder::decode_once("VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw") << "\"<br>";
//		ss << "</code>";

		ss << "<hr>Marisa/" << MARISA_VERSION << "\n";
		ss << "</body></html>";

		response->send(ss.str());
	};

	myapp.route("/usage_demo").use(func);
	myapp.route("/simple").use(Simple("Hello Marisa"));
	myapp.route("/lambda").use([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
	});

	myapp.route("/async_suspend_resume").async().use([state = 0](auto *req, Response *rsp, Context *ctx) mutable {
		if (!state) {
			std::thread ([&, ctx]{
				sleep(3);
				state = 1;
				ctx->resume();
			}).detach();

			ctx->suspend();

			puts("suspend!");
		} else {
			rsp->send("I'm back!");
		}
	});

	myapp.route("/redirect_301").use(Redirection("/simple", 301));
	myapp.route("/redirect_302").use(Redirection("/simple", 302));

	myapp.route("/error_catcher").use([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
		throw std::invalid_argument("what's up doc");
	});


	myapp.route("/sendfile").use([](auto req, auto rsp, auto ctx){
		rsp->send_file("/etc/profile");
	});

	myapp.route("/static_files/**").use(StaticFiles(".", true));
	myapp.route("/static_files2/**").use(StaticFiles("/", true));


	myapp.route("/bad_static_files/").use(StaticFiles(".", true));


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

	for (auto &url : {"no_middlewares", "inline_run1", "inline_run2",
			  "usage_demo", "simple", "lambda",
			  "async_suspend_resume",
			  "redirect_301", "redirect_302", "error_catcher",
			  "sendfile",
			  "static_files/", "static_files/CoverageTest", "/bad_static_files/",
			  "static_files/blabla000none", "static_files2/etc/passwd", "static_files2/etc/fstab",
			  "streamed_fileupload",
			  "whateveraaa", "whateverbbb", "404"}) {
		auto t = cmd_get_prefix + url;
		system(t.c_str());
	}

	system(R"(curl -v -X POST -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" 127.0.0.1:46878/inline_run2)");
//
//	system("curl -v -H 'Accept-Encoding: gzip, deflate' 127.0.0.1:46878/noyield_lambda_send_compressed");
//	system("curl -v -H 'Accept-Encoding: deflate, gzip' 127.0.0.1:46878/noyield_lambda_send_compressed");
//	system("curl -v -H 'Accept-Encoding: deflate' 127.0.0.1:46878/noyield_lambda_send_compressed");
//	system("curl -v -H 'Accept-Encoding: gzip' 127.0.0.1:46878/noyield_lambda_send_compressed");
//
	system("curl -v -X POST -F 'data=@CMakeCache.txt' 127.0.0.1:46878/streamed_fileupload");
//	system("curl -v -X POST -F 'data=@CMakeCache.txt' 127.0.0.1:46878/async_streamed_fileupload");
//
//	system("nc -nv -w 20 127.0.0.1 46878");

	myapp.stop();

	return 0;
}