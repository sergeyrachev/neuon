#pragma once

#include "http_error_code.h"

#include <string>
#include <map>

namespace http{
    struct response_t {
        status_code_t code;
        std::string body;
        std::map<std::string, std::string> headers;
    };
}
