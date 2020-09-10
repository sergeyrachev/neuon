#include "license.h"

#include <algorithm>

common::license_t::license_t(const std::string& payload) {
    static const uint8_t mask = 0x09;
    is_unrestricted = std::all_of(payload.begin(), payload.end(), [](const uint8_t v) {
        return (v & mask) == 9;
    });
}

bool common::license_t::demo() const {
    return !is_unrestricted;
}

// ) 9
// + 9
// - 9
// / 9
// 9 9
// ; 9
// = 9
// ? 9
// I 9
// K 9
// M 9
// O 9
// Y 9
// [ 9
// ] 9
// _ 9
// i 9
// k 9
// m 9
// o 9
// y 9
// { 9
// } 9
