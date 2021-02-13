/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../CommonIncludes.hpp"

namespace Marisa {
	class Request {
	public:
		struct PostData {
			std::string key, value, filename, content_type, transfer_encoding;
		};
	protected:
		void *context;

		std::string url_, method_, version_;

		std::unordered_map<std::string, std::string> url_vars_;

		std::vector<std::string_view> header_keys_cache;
		std::unordered_map<std::string_view, std::string_view> header_cache;

		std::vector<std::string_view> query_keys_cache;
		std::unordered_map<std::string_view, std::string_view> query_cache;

		std::vector<std::string_view> cookie_keys_cache;
		std::unordered_map<std::string_view, std::string_view> cookie_cache;

		std::vector<std::string> post_keys_cache;
		std::unordered_map<std::string, PostData> post_cache;

		SocketAddress<AddressFamily::Any> socket_address_cache;

		std::pair<Socket<AddressFamily::Unix, SocketType::Stream>, Socket<AddressFamily::Unix, SocketType::Stream>> input_sp;
//		int input_sp[2] = {-1, -1};
		bool input_sp_send_end_closed = false;

		const std::function<void(const std::string_view& key, const std::string_view& value, const std::string_view& filename, const std::string_view& content_type, const std::string_view& transfer_encoding)> *post_callback_ptr = nullptr;

		MHD_PostProcessor *mhd_pp = nullptr;

		static MHD_Result mhd_header_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

		static MHD_Result mhd_header_key_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

		static MHD_Result mhd_query_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

		static MHD_Result mhd_query_key_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

		static MHD_Result mhd_cookie_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

		static MHD_Result mhd_cookie_key_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

		static MHD_Result mhd_post_processor(void *cls, enum MHD_ValueKind kind, const char *key, const char *filename, const char *content_type,
					      const char *transfer_encoding, const char *data, uint64_t off, size_t size);

		static MHD_Result mhd_streamed_post_processor(void *cls, enum MHD_ValueKind kind, const char *key, const char *filename, const char *content_type,
						       const char *transfer_encoding, const char *data, uint64_t off, size_t size);

		void init();
	public:
		/**
		 * URL of the request.
		 *
		 * @return	The URL.
		 *
		 */
		const std::string& url() noexcept {
			return url_;
		}

		/**
		 * Variables in the URL.
		 *
		 * If the route specification is not a path expression (e.g. /:foo/:bar), the keys will be the index of smatches, starting from 0.
		 *
		 * @return	Key-Value pair of the variables.
		 *
		 */
		std::unordered_map<std::string, std::string>& url_vars() noexcept {
			return url_vars_;
		}

		/**
		 * Method of the request.
		 *
		 * @return	The method.
		 *
		 */
		const std::string& method() noexcept {
			return method_;
		}

		/**
		 * HTTP version string of the request.
		 *
		 * @return	The HTTP version string.
		 *
		 */
		const std::string& version() noexcept {
			return version_;
		}

		/**
		 * All headers of the request.
		 *
		 * @return	Key-Value pairs of the headers.
		 *
		 */
		const std::unordered_map<std::string_view, std::string_view>& header();

		/**
		 * Retrieve a specific header value.
		 *
		 * Some people like this style of API, so I'll provide one.
		 * Note that you can't distinguish an empty header from a nonexistent key using this function.
		 *
		 * @return	Value of the header item.
		 *
		 */
		std::string_view header(const std::string& key);

		/**
		 * All headers keys of the request. In client sequence.
		 *
		 * @return	Keys of the headers.
		 *
		 */
		const std::vector<std::string_view>& header_keys();

		/**
		 * Query strings in the URL.
		 *
		 * @return	Key-Value pairs of the headers.
		 *
		 */
		const std::unordered_map<std::string_view, std::string_view>& query();

		/**
		 * Retrieve a specific query string value.
		 *
		 * Some people like this style of API, so I'll provide one.
		 * Note that you can't distinguish an empty header from a nonexistent key using this function.
		 *
		 * @return	Value of the query string item.
		 *
		 */
		std::string_view query(const std::string& key);

		/**
		 * All query string keys of the request. In client sequence.
		 *
		 * @return	Keys of the query strings.
		 *
		 */
		const std::vector<std::string_view>& query_keys();

		/**
		 * Cookies of this request.
		 *
		 * @return	Key-Value pairs of the cookies.
		 *
		 */
		const std::unordered_map<std::string_view, std::string_view>& cookie();

		/**
		 * Retrieve a specific cookie value.
		 *
		 * Some people like this style of API, so I'll provide one.
		 * Note that you can't distinguish an empty header from a nonexistent key using this function.
		 *
		 * @return	Value of the query string item.
		 *
		 */
		std::string_view cookie(const std::string& key);

		/**
		 * All cookie keys of the request. In client sequence.
		 *
		 * @return	Keys of the cookies.
		 *
		 */
		const std::vector<std::string_view>& cookie_keys();

		/**
		 * POST data of this request.
		 *
		 * This function is only available in normal mode. For streamed mode, use read_post().
		 *
		 * @return	Key-Value[s] pairs of the POST data.
		 *
		 */
		std::unordered_map<std::string, PostData>& post() noexcept {
			return post_cache;
		}

		/**
		 * All POST data keys of the request. In client sequence.
		 *
		 * This function is only available in normal mode. For streamed mode, use read_post().
		 *
		 * @return	Keys of the POST data.
		 *
		 */
		std::vector<std::string>& post_keys() noexcept {
			return post_keys_cache;
		}

		/**
		 * Socket address of this connection.
		 *
		 * Only useful if the server is not behind a reverse proxy.
		 *
		 * @return	The socket address.
		 *
		 */
		const SocketAddress<AddressFamily::Any>& socket_address();

		/**
		 * Read raw POST data from client.
		 *
		 * Only available in streamed mode.
		 * This is a blocking operation.
		 *
		 * @return	Raw POST data chunk. Empty for EOF.
		 *
		 */
		std::vector<uint8_t> read();

		/**
		 * Read raw POST data from client, parse it, and use a callback to tell the contents.
		 *
		 * Only available in streamed mode.
		 * This is a blocking operation.
		 *
		 * @param	callback	Callback to retrieve the POST data contents.
		 *
		 * @return	Indicates if there is more data to be read.
		 *
		 */
		bool read_post(const std::function<void(const std::string_view& key, const std::string_view& value, const std::string_view& filename, const std::string_view& content_type, const std::string_view& transfer_encoding)>& callback);

		Request(void *__context, const char *__mhd_url, const char *__mhd_method, const char *__mhd_version) :
			context(__context), url_(__mhd_url), method_(__mhd_method), version_(__mhd_version) {

		}

		~Request();

	};

	class RequestExposed : public Request {
	public:
		using Request::mhd_post_processor;
		using Request::mhd_pp;
		using Request::input_sp;
		using Request::input_sp_send_end_closed;
		using Request::init;
	};
}