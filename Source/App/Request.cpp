/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "../CommonIncludes.hpp"
#include "../Util/Util.hpp"
#include "Request.hpp"
#include "Context.hpp"


using namespace Marisa;

#define logger		((Context *)context)->app->logger_internal
#define logger_sf	((Context *)(((Request *)cls)->context))->app->logger_internal

static const char ModuleName[] = "Request";

void Request::init() {
	auto *ctx = (Context *)context;

	if (ctx->streamed()) {
		input_sp = socket_pair<SocketType::Stream>();
		logger->debug(R"([{} @ {:x}] input_sp fds: {}, {})", ModuleName, (intptr_t) this, input_sp.first.fd(), input_sp.second.fd());

		input_sp.first.set_nonblocking();
		input_sp.second.set_nonblocking();
	}
}

const std::unordered_map<std::string_view, std::string_view> &Request::header() {
	if (header_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_HEADER_KIND, &mhd_header_cb, this);
	}

	return header_cache;
}

std::string_view Request::header(const std::string& key) {
	if (header_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_HEADER_KIND, &mhd_header_cb, this);
	}

	auto it = header_cache.find(key);
	if (it != header_cache.end()) {
		return it->second;
	} else {
		return {};
	}
}

const std::vector<std::string_view> &Request::header_keys() {
	if (header_keys_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_HEADER_KIND, &mhd_header_key_cb, this);
	}

	return header_keys_cache;
}

const std::unordered_map<std::string_view, std::string_view> &Request::query() {
	if (query_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_GET_ARGUMENT_KIND, &mhd_query_cb, this);
	}

	return query_cache;
}

std::string_view Request::query(const std::string &key) {
	if (query_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_GET_ARGUMENT_KIND, &mhd_query_cb, this);
	}

	auto it = query_cache.find(key);
	if (it != query_cache.end()) {
		return it->second;
	} else {
		return {};
	}
}

const std::vector<std::string_view> &Request::query_keys() {
	if (query_keys_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_GET_ARGUMENT_KIND, &mhd_query_key_cb, this);
	}

	return query_keys_cache;
}

const std::unordered_map<std::string_view, std::string_view> &Request::cookie() {
	if (cookie_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_GET_ARGUMENT_KIND, &mhd_cookie_cb, this);
	}

	return cookie_cache;
}

std::string_view Request::cookie(const std::string &key) {
	if (cookie_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_GET_ARGUMENT_KIND, &mhd_cookie_cb, this);
	}

	auto it = cookie_cache.find(key);
	if (it != cookie_cache.end()) {
		return it->second;
	} else {
		return {};
	}
}

const std::vector<std::string_view> &Request::cookie_keys() {
	if (cookie_keys_cache.empty()) {
		MHD_get_connection_values(((Context *)context)->mhd_conn, MHD_GET_ARGUMENT_KIND, &mhd_cookie_key_cb, this);
	}

	return cookie_keys_cache;
}

const SocketAddress<AddressFamily::Any>& Request::socket_address() {
	if (socket_address_cache.family() != AddressFamily::IPv4 && socket_address_cache.family() != AddressFamily::IPv6) {
		auto *sa = MHD_get_connection_info(((Context *) context)->mhd_conn,MHD_CONNECTION_INFO_CLIENT_ADDRESS)->client_addr;

		if (sa->sa_family == AF_INET) {
			memcpy(socket_address_cache.raw(), sa, sizeof(sockaddr_in));
		} else {
			memcpy(socket_address_cache.raw(), sa, sizeof(sockaddr_in6));
		}
	}

	return socket_address_cache;
}

bool Request::read_post(
	const std::function<void(const std::string_view &, const std::string_view &, const std::string_view &, const std::string_view &, const std::string_view &)> &callback) {
	post_callback_ptr = &callback;

	if (!mhd_pp)
		mhd_pp = MHD_create_post_processor(((Context *)context)->mhd_conn, 4096, &mhd_streamed_post_processor, this);

	auto buf = read();
	if (buf.empty()) {
		logger->debug("[{} @ {:x}] read_post end", ModuleName, (intptr_t)this);

		if (mhd_pp) {
			MHD_destroy_post_processor(mhd_pp);
			mhd_pp = nullptr;
		}
		return false;
	}

	MHD_post_process(mhd_pp, reinterpret_cast<const char *>(buf.data()), buf.size());

	return true;
}

std::vector<uint8_t> Request::read() {
	std::vector<uint8_t> ret(4096);

	while (1) {
		auto rc_recv = input_sp.second.recv(ret.data(), ret.size(), 0);

		if (rc_recv > 0) {
			input_sp.second.set_nonblocking();
			ret.resize(rc_recv);
			break;
		} else if (rc_recv == 0) {
			ret.resize(0);
			try {
				input_sp.second.shutdown();
			} catch (std::exception &e) {
				logger->warn("[{} @ {:x}] shutdown failed: {}", ModuleName, (intptr_t)this, e.what());

			}
			break;
		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				input_sp.second.set_nonblocking(false);

				((Context *) context)->resume_connection();
//				std::this_thread::yield();
			} else {
				ret.resize(0);
				break;
			}
		}

	}

	logger->trace("[{} @ {:x}] read={}", ModuleName, (intptr_t)this, ret.size());

	return ret;
}

Request::~Request() {
	///close(input_sp[1]);
}

MHD_Result Request::mhd_header_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
	auto *ctx = (Request *)cls;

	logger_sf->trace("[{} @ {:x}] mhd_cookie_key_cb: kind: {}, key: {}@{}, value: {}@{}",
			 ModuleName, (intptr_t)ctx, kind, key, (intptr_t)key, value, (intptr_t)value);

	ctx->header_cache.insert({key, value});

	return MHD_YES;
}

MHD_Result Request::mhd_header_key_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
	auto *ctx = (Request *)cls;

	logger_sf->trace("[{} @ {:x}] mhd_header_key_cb: kind: {}, key: {}@{}, value: {}@{}",
			 ModuleName, (intptr_t)ctx, kind, key, (intptr_t)key, value, (intptr_t)value);

	ctx->header_keys_cache.emplace_back(key);

	return MHD_YES;
}

MHD_Result Request::mhd_query_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
	auto *ctx = (Request *)cls;

	logger_sf->trace("[{} @ {:x}] mhd_query_cb: kind: {}, key: {}@{}, value: {}@{}",
			 ModuleName, (intptr_t)ctx, kind, key, (intptr_t)key, value, (intptr_t)value);

	ctx->query_cache.insert({key, value});

	return MHD_YES;
}

MHD_Result Request::mhd_query_key_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
	auto *ctx = (Request *)cls;

	logger_sf->trace("[{} @ {:x}] mhd_query_key_cb: kind: {}, key: {}@{}, value: {}@{}",
			 ModuleName, (intptr_t)ctx, kind, key, (intptr_t)key, value, (intptr_t)value);

	ctx->query_keys_cache.emplace_back(key);

	return MHD_YES;
}

MHD_Result Request::mhd_cookie_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
	auto *ctx = (Request *)cls;

	logger_sf->trace("[{} @ {:x}] mhd_cookie_cb: kind: {}, key: {}@{}, value: {}@{}",
			 ModuleName, (intptr_t)ctx, kind, key, (intptr_t)key, value, (intptr_t)value);

	ctx->cookie_cache.insert({key, value});

	return MHD_YES;
}

MHD_Result Request::mhd_cookie_key_cb(void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
	auto *ctx = (Request *)cls;

	logger_sf->trace("[{} @ {:x}] mhd_cookie_key_cb: kind: {}, key: {}@{}, value: {}@{}",
			 ModuleName, (intptr_t)ctx, kind, key, (intptr_t)key, value, (intptr_t)value);

	ctx->cookie_keys_cache.emplace_back(key);

	return MHD_YES;
}

MHD_Result
Request::mhd_post_processor(void *cls, enum MHD_ValueKind kind, const char *key, const char *filename, const char *content_type, const char *transfer_encoding, const char *data,
			    uint64_t off, size_t size) {
	auto *ctx = (Request *)cls;

	if (key) {
		if (!ctx->post_keys_cache.empty() && ctx->post_keys_cache.back() != key) {
			ctx->post_keys_cache.emplace_back(key);
		}

		auto &pd = ctx->post_cache[key];

		if (size)
			pd.value.insert(pd.value.end(), data, data + size);

		if (content_type && pd.content_type.empty())
			pd.content_type = content_type;

		if (transfer_encoding && pd.transfer_encoding.empty())
			pd.transfer_encoding = transfer_encoding;
	}

	logger_sf->trace("[{} @ {:x}] mhd_post_processor: kind: {}, key: {}, filename: {}, content_type: {}, transfer_encoding: {}, data: {}, off: {}, size: {}",
			 ModuleName, (intptr_t)ctx, kind, key, filename, content_type, transfer_encoding, data, off, size);


	return MHD_YES;
}

MHD_Result Request::mhd_streamed_post_processor(void *cls, enum MHD_ValueKind kind, const char *key, const char *filename, const char *content_type, const char *transfer_encoding,
						const char *data, uint64_t off, size_t size) {
	auto *ctx = (Request *)cls;


	(*ctx->post_callback_ptr)(key ? key : std::string_view(),
				  size ? std::string_view(data, size) : std::string_view(),
				  filename ? filename : std::string_view(),
				  content_type ? content_type : std::string_view(),
				  transfer_encoding ? transfer_encoding : std::string_view());

	logger_sf->trace("[{} @ {:x}] mhd_streamed_post_processor: kind: {}, key: {}, filename: {}, content_type: {}, transfer_encoding: {}, data: {}, off: {}, size: {}",
			 ModuleName, (intptr_t)ctx, kind, key, filename, content_type, transfer_encoding, data, off, size);


	return MHD_YES;
}

