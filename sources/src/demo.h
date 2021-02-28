#pragma once

#include <stdexcept>

namespace common{
    class demo_t{
    public:
        explicit demo_t(std::string birthday, uint32_t days = 30);

        bool has_expired;
    };
}
