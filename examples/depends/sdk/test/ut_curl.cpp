#include "gtest/gtest.h"

#include "curl.h"

class uncovered_curl_t : public http::curl_t{
public:
    using http::curl_t::curl_t;
    using http::curl_t::endpoint;
    using http::curl_t::write_callback;
    using http::curl_t::read_callback;
    using http::curl_t::header_callback;
    using http::curl_t::make_headers_list;
    using http::curl_t::sanitize_response;
    using http::curl_t::sanitize_entrypoint_url;
    using http::curl_t::sanitize_base_url;
    using http::curl_t::payload_t;
};

TEST(curl, build_endpoint){
    const std::string base_url("http://localhost:1234/service");
    auto endpoint = uncovered_curl_t::endpoint(base_url, "/object");
    ASSERT_EQ("http://localhost:1234/service/object", endpoint);
}

TEST(curl, sanitize_base_url_slash){
    ASSERT_EQ("http://localhost:1234/service", uncovered_curl_t::sanitize_base_url("http://localhost:1234/service/"));
}

TEST(curl, sanitize_base_url_no_slash){
    ASSERT_EQ("http://localhost:1234/service", uncovered_curl_t::sanitize_base_url("http://localhost:1234/service"));
}

TEST(curl, sanitize_entrypoint_url_slash){
    ASSERT_EQ("/service", uncovered_curl_t::sanitize_entrypoint_url("/service"));
}

TEST(curl, sanitize_entrypoint_url_no_slash){
    ASSERT_EQ("/service", uncovered_curl_t::sanitize_entrypoint_url("service"));
}

TEST(curl, write_callback_append_string){
    uint8_t data[] = "0123";

    std::string str;
    auto ret = uncovered_curl_t::write_callback(data, 1, 1, &str);
    ASSERT_EQ("0", str);
    ASSERT_EQ(1, ret);

    ret = uncovered_curl_t::write_callback(data, 1, 2, &str);
    ASSERT_EQ("001", str);
    ASSERT_EQ(2, ret);

    ret = uncovered_curl_t::write_callback(data, 2, 1, &str);
    ASSERT_EQ("00101", str);
    ASSERT_EQ(2, ret);
}

TEST(curl, read_callback_partial_reading){
    std::string data("0123456789");
    uncovered_curl_t::payload_t payload{
        data.c_str(), data.size()
    };

    std::vector<uint8_t> output_buffer(1000);

    auto ret = uncovered_curl_t::read_callback(output_buffer.data(), 1, 1, &payload);
    ASSERT_EQ('0', output_buffer[0]);
    ASSERT_EQ(1, ret);
    ASSERT_EQ(9, payload.rest);

    ret = uncovered_curl_t::read_callback(output_buffer.data(), 2, 4, &payload);
    ASSERT_EQ('1', output_buffer[0]);
    ASSERT_EQ(8, ret);
    ASSERT_EQ(1, payload.rest);

    ret = uncovered_curl_t::read_callback(output_buffer.data(), 4, 2, &payload);
    ASSERT_EQ('9', output_buffer[0]);
    ASSERT_EQ(1, ret);
    ASSERT_EQ(0, payload.rest);
}

TEST(curl, header_callback){
    std::map<std::string, std::string> headers;
    std::string data("Name: value");
    auto ret = uncovered_curl_t::header_callback(data.data(), 1, data.size(), &headers);

    ASSERT_EQ(11, ret);
    ASSERT_NE(headers.end(), headers.find("Name"));
    ASSERT_EQ("value", headers["Name"]);


    std::string bad_header("Garbage");
    ret = uncovered_curl_t::header_callback(bad_header.data(), 1, bad_header.size(), &headers);
    ASSERT_EQ(7, ret);
    ASSERT_EQ(1, headers.size());

    std::string separated_value("Header : :Complex:Value:");
    ret = uncovered_curl_t::header_callback(separated_value.data(), 1, separated_value.size(), &headers);
    ASSERT_EQ(24, ret);
    ASSERT_EQ(":Complex:Value:", headers["Header"]);

    std::string empty_value("Empty:");
    ret = uncovered_curl_t::header_callback(empty_value.data(), 1, empty_value.size(), &headers);
    ASSERT_EQ(6, ret);
    ASSERT_EQ("", headers["Empty"]);
}

TEST(curl, expect_header){
    std::map<std::string, std::string> headers;

    auto header_list(uncovered_curl_t::make_headers_list(headers));
    ASSERT_EQ("Expect:", std::string(header_list->data));
}

TEST(curl, sanitize_success){
    http::response_t response;
    ASSERT_NO_THROW(uncovered_curl_t::sanitize_response(CURLE_OK, response));
}

TEST(curl, sanitize_fail){
    http::response_t response;
    ASSERT_THROW(uncovered_curl_t::sanitize_response(CURLE_AGAIN, response), http::transport_error_t);
}
