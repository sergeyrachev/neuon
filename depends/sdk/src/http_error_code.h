#pragma once

#include <cstdint>

namespace http {
    enum class status_code_t : uint32_t {
        ok = 200,
        created = 201,
        no_content = 204,
        bad_request = 400,
        not_found = 404,
        conflict = 409,
        gone = 410,
        internal_server_error = 500
    };
}
