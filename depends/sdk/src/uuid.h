#pragma once

#include <string>

namespace posix{
    class uuid_t{
    public:
        uuid_t();
        std::string str() const;
    private:
        std::string value;
    };
}
