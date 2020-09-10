#include "gtest/gtest.h"

#include "ntp.h"

using namespace datetime;
TEST(ntp, build){

    auto ntp_value = pack({0x01020304, 0x05060708});

    EXPECT_EQ(0x0102030405060708, ntp_value);
}

TEST(ntp, unpack){

    auto ntp = unpack(0x0102030405060708);
    EXPECT_EQ(0x01020304, ntp.seconds);
    EXPECT_EQ(0x05060708, ntp.fraction);
}

TEST(ntp, from_iso8601){

    ntp_t value = from_iso8601("1900-01-01 00:00:01");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 10:00:00");
    ASSERT_EQ(36000, value.seconds);

    value = from_iso8601("1900-01-02 00:00:00");
    ASSERT_EQ(86400, value.seconds);

    value = from_iso8601("1900-01-01 00:00:00.5");
    ASSERT_EQ(0, value.seconds);
    ASSERT_EQ(value.fractions_per_second / 2,  value.fraction);

    value = from_iso8601("1900-01-01 00:00:00.000001");
    ASSERT_EQ(0, value.seconds);
    ASSERT_EQ(value.fractions_per_second / 1000000, value.fraction);
}

TEST(ntp, with_delimiter){
    ntp_t value = from_iso8601("1900-01-01T00:00:01");
    ASSERT_EQ(1, value.seconds);
}

TEST(ntp, bad_hour_value){
    EXPECT_THROW(from_iso8601("1900-01-01 100:99:01"), std::exception);
}

TEST(ntp, bad_minute_value){
    EXPECT_THROW(from_iso8601("1900-01-01 00:100:01"), std::exception);
}

TEST(ntp, bad_second_value){
    EXPECT_NO_THROW(from_iso8601("1900-01-01 00:00:100000"));
}

TEST(ntp, bad_value_good_format){
    EXPECT_NO_THROW(from_iso8601("9999-12-31 99:99:99.999999"));
}

TEST(ntp, bad_input){
        EXPECT_THROW(from_iso8601("A1900-01-01 99:99:01"), std::exception);
}

TEST(ntp, from_iso8601_silently_ignore_zone){

    ntp_t value = from_iso8601("1900-01-01 00:00:01Z");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01Z");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01-01");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01-01:00");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01-23:30");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01+01");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01+01:02");
    ASSERT_EQ(1, value.seconds);

    value = from_iso8601("1900-01-01 00:00:01+0100");
    ASSERT_EQ(1, value.seconds);
}
