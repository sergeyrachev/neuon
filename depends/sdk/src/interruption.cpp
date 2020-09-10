// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "interruption.h"



bool threads::interruption_t::done() {
    std::lock_guard<std::mutex> lck(guard);
    return has_signaled;
}

void threads::interruption_t::wait(const std::chrono::milliseconds &delay) {
    std::unique_lock<std::mutex> lck(guard);
    condition.wait_for(lck, delay, [this](){
        return has_signaled;
    });
}

void threads::interruption_t::interrupt() {
    std::unique_lock<std::mutex> lck(guard);
    has_signaled = true;
    // Is not really needed but safer:
    // http://en.cppreference.com/w/cpp/thread/condition_variable/notify_one
    lck.unlock();
    condition.notify_all();
}
