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

#include "../Marisa.hpp"

#include <iostream>

using namespace Marisa::Application;
using namespace Middlewares;


class blocking_class_send : public Middleware {
public:
	void handler() override {
		try {
			std::ostringstream ss;
			ss << "<html><head>\n"
			   << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
			   << "<title>""</title>\n"
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


			for (auto &it : request->header) {
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
		} catch (...) {

		}
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<blocking_class_send>();
	}
};

class blocking_class_write : public Middleware {
public:
	void handler() override {
		try {
			response->write("Hello Marisa");
			response->end();
		} catch (...) {

		}
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<blocking_class_write>();
	}
};

class file_upload : public Middleware {
public:
	void handler() override {
		Mullet mp;
		mp.set_headers(request->headers_lowercase);

		Base64::Encoder b64_enc;

		std::string encoded;
		while (true) {
			auto buf = request->read(16);
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

		response->send(encoded);
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<file_upload>();
	}
};

int main() {

	LogD("Hey guys, this is a debug log\n");

	Date d0("Fri, 05 Jul 2019 07:47:57 GMT");
	Date dNow;

	std::cout << dNow.toString() << "\n";
	std::cout << dNow.toDateString() << "\n";
	std::cout << dNow.toGMTString() << "\n";
	std::cout << dNow.toGoodString() << "\n";
	std::cout << dNow.toISOString() << "\n";

	Date d1;

	d1.assign_jstime(0);
	d1.assign_unixtime(0);

	timespec ts1;
	clock_gettime(CLOCK_REALTIME, &ts1);
	d1.assign_timespec(&ts1);

	const char static_arr[] = "Hello Marisa";
	std::vector<uint8_t> vec;
	vec.insert(vec.end(), static_arr, static_arr+sizeof(static_arr)-1);
	App myapp;

	myapp.route("/blocking_class_send").on("GET").use(blocking_class_send());
	myapp.route("/blocking_class_write").on("GET").use(blocking_class_write());
	myapp.route("/blocking_simple").on("GET").use(Simple("Hello Marisa"));
	myapp.route("/blocking_lambda").on("GET").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
	}));

	myapp.route("/async_lambda_send").nonblocking().on("GET").use(Lambda([&](auto req, auto rsp, auto ctx){
		rsp->send(vec);
	}));

	myapp.route("/async_lambda_write").nonblocking().on("GET").use(Lambda([&](auto req, auto rsp, auto ctx){
		rsp->write("Hello Marisa\n");
		rsp->write(vec);
		rsp->write(static_arr, static_arr+sizeof(static_arr)-1);
		rsp->end();
	}));

	myapp.route("/async_lambda_post").nonblocking().on("POST").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->write("Hello Marisa! Post data:");
		auto d = req->body();
		rsp->write(d.data(), d.size());
		rsp->end();
	}));

	myapp.route("/noyield_lambda_send_compressed").no_yield().on("GET").use(Compression()).use(Lambda([&](auto req, auto rsp, auto ctx){
		rsp->send(static_arr, static_arr+sizeof(static_arr)-1);
	}));

	myapp.route("/redirect_301").no_yield().on("*").use(Redirection("/async_lambda_send", 301));
	myapp.route("/redirect_302").no_yield().on("*").use(Redirection("/async_lambda_send", 302));

	myapp.route("/error_catcher").on("*").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
		throw std::invalid_argument("what's up doc");
	}));

	myapp.route("/async_error_catcher").nonblocking().on("*").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
		throw std::invalid_argument("what's up doc");
	}));

	myapp.route("/noyield_error_catcher").no_yield().on("*").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
		throw std::invalid_argument("what's up doc");
	}));

	myapp.route("/sendfile").nonblocking().on("GET").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->type = "text/plain";
		rsp->send_file("/etc/profile");
	}));

	myapp.route("/async_sendfile").nonblocking().on("GET").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->type = "text/plain";
		rsp->send_file("/etc/profile");
	}));

	myapp.route("/noyield_sendfile").no_yield().on("GET").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->type = "text/plain";
		rsp->send_file("/etc/profile");
	}));

	myapp.route("/static_files/*").no_yield().on("GET").use(StaticFiles(".", true));


	myapp.route("/blocking_streamed_fileupload").stream().on("POST").use(file_upload());
	myapp.route("/async_streamed_fileupload").nonblocking().stream().on("POST").use(file_upload());

	myapp.route(std::regex("^whatever")).no_yield().on("GET").use(Simple("whatever!!"));

	myapp.Config.connection.timeout_seconds = 15;
	myapp.listen(46878);
	myapp.listen("127.0.0.1", 47000);
	myapp.listen_ssl(46879, [](auto& ssl_ctx){
		ssl_ctx.use_certificate_file("/etc/ssl/certs/ssl-cert-snakeoil.pem", boost::asio::ssl::context_base::file_format::pem);
		ssl_ctx.use_private_key_file("/etc/ssl/private/ssl-cert-snakeoil.key", boost::asio::ssl::context_base::file_format::pem);
	});
	myapp.listen_ssl("127.0.0.1", 47001, [](auto& ssl_ctx){
		ssl_ctx.use_certificate_file("/etc/ssl/certs/ssl-cert-snakeoil.pem", boost::asio::ssl::context_base::file_format::pem);
		ssl_ctx.use_private_key_file("/etc/ssl/private/ssl-cert-snakeoil.key", boost::asio::ssl::context_base::file_format::pem);
	});
	unlink("/tmp/.marisa_socket");
	myapp.listen_unix("/tmp/.marisa_socket");

	std::thread ([&]() {
		myapp.run(2);
		exit(0);
	}).detach();

	sleep(2);

	std::string cmd_get_prefix = "curl -v -H 'Cookie: aaa=bbb;ccc=ddd' 127.0.0.1:46878/";
	std::string cmd_get_prefix_ssl = "curl -v --insecure -H 'Cookie: aaa=bbb;ccc=ddd' https://127.0.0.1:46879/";

	for (auto &url : {"blocking_class_send", "blocking_class_write", "blocking_simple", "blocking_lambda",
		   "async_lambda_send", "async_lambda_write", "error_catcher", "async_error_catcher",
		   "noyield_error_catcher", "sendfile", "async_sendfile", "noyield_sendfile", "static_files/",
		   "static_files/CMakeCache.txt", "redirect_301", "redirect_302", "whateveraaa", "whateverbbb", "404"}) {
		auto t = cmd_get_prefix + url;
		system(t.c_str());
		auto ts = cmd_get_prefix_ssl + url;
		system(ts.c_str());
	}

	system(R"(curl -v -X POST -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" 127.0.0.1:46878/async_lambda_post)");

	system("curl -v -H 'Accept-Encoding: gzip, deflate' 127.0.0.1:46878/noyield_lambda_send_compressed");
	system("curl -v -H 'Accept-Encoding: deflate, gzip' 127.0.0.1:46878/noyield_lambda_send_compressed");
	system("curl -v -H 'Accept-Encoding: deflate' 127.0.0.1:46878/noyield_lambda_send_compressed");
	system("curl -v -H 'Accept-Encoding: gzip' 127.0.0.1:46878/noyield_lambda_send_compressed");

	system("curl -v -X POST -F 'data=@CMakeCache.txt' 127.0.0.1:46878/blocking_streamed_fileupload");
	system("curl -v -X POST -F 'data=@CMakeCache.txt' 127.0.0.1:46878/async_streamed_fileupload");

	system("nc -nv -w 20 127.0.0.1 46878");

	myapp.stop();

	while (true)
		sleep(-1);
}