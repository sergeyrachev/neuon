#pragma once
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace threads{
    class interruption_t {
    public:
        bool done();
        void wait(const std::chrono::milliseconds &delay);
        void interrupt();

    private:
        std::mutex guard;
        std::condition_variable condition;
        bool has_signaled{false};
    };
}



