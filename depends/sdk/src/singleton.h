#pragma once

namespace algorithm{

    template<typename T>
    class singleton_t {
    private:
        singleton_t() = default;
    public:
        static T& instance() {
            static T instance;
            return instance;
        }
    };
}
