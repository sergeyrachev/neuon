#pragma once

#include <cstdint>
#include <stdexcept>

namespace common{
    class demo_t{
    public:
        explicit demo_t(std::string birthday, uint32_t days = 120);

        bool has_expired;
    };
}
