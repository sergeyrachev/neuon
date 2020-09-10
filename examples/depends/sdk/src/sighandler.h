#pragma once

#include <functional>
#include <csignal>

namespace posix{
    template<int signal>
    class sighandler_t{
    public:
        sighandler_t() = delete;
        static void assign(const std::function<void(int)>& f){
            functor = f;
            std::signal(signal, action);
        }
    private:
        static void action(int sig){
            functor(sig);
        }
        static std::function<void(int)> functor;
    };

    template<int s>
    std::function<void(int)> sighandler_t<s>::functor;
}
