#pragma once

#include <stdexcept>

namespace http{
    class transport_error_t : public std::runtime_error{
    public:
        transport_error_t(const std::string& arg) : std::runtime_error(arg){};
        using std::runtime_error::what;
    };
}
