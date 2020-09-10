#pragma once

#include "http_error_code.h"
#include "response.h"
#include "transport_error.h"

#include <cstdint>
#include <map>
#include <string>
#include <memory>
#include <mutex>

#include <curl/curl.h>

namespace http{
    class curl_t {
    public:
        explicit curl_t(const std::string &base_url);

        response_t post(const std::string &url, const std::string &body = std::string(), const std::map<std::string, std::string> &headers = std::map<std::string, std::string>());
        response_t get(const std::string &url, const std::map<std::string, std::string> &headers = std::map<std::string, std::string>());
        response_t put(const std::string &url, const std::string &body = std::string(), const std::map<std::string, std::string> &headers = std::map<std::string, std::string>());

    protected:
        response_t execute(CURL *curl, const std::string &url, const std::map<std::string, std::string> &headers);
        static std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> make_headers_list(const std::map<std::string, std::string> &headers);
        static size_t write_callback(const void *const data, size_t size, size_t nmemb, void *userdata);
        static size_t read_callback(void *const data, size_t size, size_t nmemb, void *userdata);
        static size_t header_callback(const void *const data, size_t size, size_t nmemb, void *userdata);
        static std::string endpoint(const std::string &base_url, const std::string &url);
        static std::string sanitize_base_url(const std::string url);
        static std::string sanitize_entrypoint_url(const std::string url);
        static response_t sanitize_response(const CURLcode code, const response_t& response);

    protected:
        struct payload_t {
            const char *value;
            size_t rest;
        };

    private:
        typedef std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl_ptr;
    private:
        std::mutex curl_guard;
        curl_ptr curl;
        std::string base_url;
    };
}
