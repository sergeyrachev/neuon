#pragma once

#include <string>

namespace common {
    class license_t{
    public:
        license_t(const std::string& payload);

        bool demo() const;

    private:
        bool is_unrestricted{false};
    };
}
