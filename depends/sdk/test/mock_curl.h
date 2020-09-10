#pragma once

#include "gmock/gmock.h"

#include "response.h"

#include <string>

namespace mock {
    class curl_t {
    public:
        curl_t();

        ~curl_t();

        MOCK_METHOD2(get, http::response_t(
            const std::string &url,
            const std::map<std::string, std::string> &headers));

        MOCK_METHOD3(post, http::response_t(
            const std::string &url,
            const std::string &body,
            const std::map<std::string, std::string> &headers));

        MOCK_METHOD3(put, http::response_t(
            const std::string &url,
            const std::string &body,
            const std::map<std::string, std::string> &headers));

        MOCK_METHOD1(get, http::response_t(
            const std::string &url));

        MOCK_METHOD2(post, http::response_t(
            const std::string &url,
            const std::string &body));

        MOCK_METHOD1(put, http::response_t(
            const std::string &url));

    };
}
