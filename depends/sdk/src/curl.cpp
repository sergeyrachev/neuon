#include "curl.h"

#include "transport_error.h"

#include <boost/algorithm/string.hpp>

using namespace http;

curl_t::curl_t(const std::string &base_url)
    : curl(curl_easy_init(), &curl_easy_cleanup)
    , base_url(sanitize_base_url(base_url)) {

}

response_t curl_t::post(const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers) {
    std::lock_guard<std::mutex> lck(curl_guard);

    curl_easy_setopt(curl.get(), CURLOPT_POST, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE, body.size());

    return execute(curl.get(), endpoint(base_url, sanitize_entrypoint_url(url)), headers);
}

response_t curl_t::get(const std::string &url, const std::map<std::string, std::string> &headers) {
    std::lock_guard<std::mutex> lck(curl_guard);

    return execute(curl.get(), endpoint(base_url,  sanitize_entrypoint_url(url)), headers);
}

response_t curl_t::put(const std::string &url, const std::string &body, const std::map<std::string, std::string> &headers) {
    std::lock_guard<std::mutex> lck(curl_guard);

    curl_easy_setopt(curl.get(), CURLOPT_PUT, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_READFUNCTION, read_callback);

    payload_t payload{body.c_str(), body.length()};
    curl_easy_setopt(curl.get(), CURLOPT_READDATA, &payload);
    curl_easy_setopt(curl.get(), CURLOPT_INFILESIZE, static_cast<int64_t>(payload.rest));

    return execute(curl.get(), endpoint(base_url, sanitize_entrypoint_url(url)), headers);
}

response_t curl_t::execute(CURL *curl, const std::string &url, const std::map<std::string, std::string> &headers) {
    response_t ret{http::status_code_t::internal_server_error, {}, {}};

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret.body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ret.headers);

    auto headers_list = make_headers_list(headers);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list.get());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    CURLcode code = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &ret.code);
    curl_easy_reset(curl);

    return sanitize_response(code, ret);
}

std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> curl_t::make_headers_list(const std::map<std::string, std::string> &headers) {

    // Disable 100 Continue response
    curl_slist *headerList = curl_slist_append(NULL, "Expect:");
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        const auto &header = *it;
        const std::string headerString = header.first + ": " + header.second;
        headerList = curl_slist_append(headerList, headerString.c_str());
    }

    return {headerList, &curl_slist_free_all};
}

size_t curl_t::write_callback(const void *const data, size_t size, size_t nmemb, void *userdata) {
    auto &body = *static_cast<std::string *>(userdata);
    size_t consumed = (size * nmemb);
    body.append(static_cast<const char *>(data), consumed);
    return consumed;
}

size_t curl_t::read_callback(void *const data, size_t size, size_t nmemb, void *userdata) {

    auto &payload = *static_cast<payload_t *>(userdata);

    size_t curl_size = size * nmemb;
    size_t copy_size = std::min(payload.rest, curl_size);
    std::memcpy(data, payload.value, copy_size);

    payload.rest -= copy_size;
    payload.value += copy_size;

    return copy_size;
}

size_t curl_t::header_callback(const void *const data, size_t size, size_t nmemb, void *userdata) {
    size_t consumed = size * nmemb;
    std::string header_data(static_cast<const char *>(data), consumed);
    size_t header_delimiter = header_data.find_first_of(':');

    if (std::string::npos != header_delimiter) {

        std::string key = header_data.substr(0, header_delimiter);
        std::string value = header_data.substr(header_delimiter + 1);

        boost::trim(key);
        boost::trim(value);

        auto &headers = *static_cast<std::map<const std::string, std::string> *>(userdata);
        headers[key] = value;
    }
    return consumed;
}

std::string curl_t::endpoint(const std::string &base_url, const std::string &url) {
    assert(*base_url.rbegin() != '/' && "Base url must not end with slash");
    assert(*url.begin() == '/' && "Entrypoint url must begin with slash");
    return base_url + url;
}

response_t curl_t::sanitize_response(const CURLcode code, const response_t &response) {
    if(code != CURLE_OK) {
        throw transport_error_t("CURL request failed with code: " + std::to_string(static_cast<long long unsigned int>(code))); // GCC 4.4 has limited overloading resolution
    }
    return response;
}

std::string curl_t::sanitize_base_url(const std::string url) {
    if(*url.rbegin() == '/'){
        return url.substr(0, url.size() - 1);
    }
    return url;
}

std::string curl_t::sanitize_entrypoint_url(const std::string url) {
    if(*url.begin() != '/'){
        return "/" + url;
    }
    return url;
}
