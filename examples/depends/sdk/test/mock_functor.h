#pragma once

#include "gmock/gmock.h"

namespace mock{
    template<typename A>
    struct functor_t{
        functor_t() = default;
        ~functor_t() = default;
        MOCK_METHOD1_T(call, void(const A&));
    };
}
