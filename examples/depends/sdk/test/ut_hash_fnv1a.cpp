#include "gtest/gtest.h"

#include "hash_fnv1a.h"

TEST(hash, compile_time){
    using algorithm::operator"" _H;
    constexpr std::uint32_t check = "0123456789ABCDEF"_H;
    ASSERT_EQ(141695047u, check);
}
