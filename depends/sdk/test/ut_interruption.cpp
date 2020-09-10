#include "gtest/gtest.h"

#include "interruption.h"

#include <chrono>
#include <future>

using threads::interruption_t ;

TEST(interruption, interrupt){
    interruption_t interruption;

    ASSERT_FALSE(interruption.done());
    interruption.interrupt();
    ASSERT_TRUE(interruption.done());
}

TEST(interruption, wait){
    interruption_t interruption;

    auto waiter = std::async(std::launch::async, [&interruption](){
        interruption.wait(std::chrono::milliseconds(10000));
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ASSERT_FALSE(interruption.done());
    interruption.interrupt();
    ASSERT_TRUE(interruption.done());
    // Block explicitly to wait return from async routine
    waiter.get();
}
