#pragma once

#include <cstdint>
#include <string>

namespace datetime{
    struct ntp_t{
        uint32_t seconds;
        uint32_t fraction;

        static const uint32_t fractions_per_second = 0xffffffff;

    };

    uint64_t pack(const ntp_t &value);
    ntp_t unpack(uint64_t value);

    ntp_t from_iso8601(const std::string &str);
}

