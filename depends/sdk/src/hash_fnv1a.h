#pragma once

#include <cstddef>
#include <cstdint>


namespace algorithm{
    namespace detail{
        constexpr std::uint32_t fnv1a_32(const char* s, std::size_t count) {
            return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ static_cast<uint32_t >(s[count])) * 16777619u;
        }
    }

    constexpr uint32_t operator "" _H(const  char *s, std::size_t count) {
        return detail::fnv1a_32(s, count);
    }

}
