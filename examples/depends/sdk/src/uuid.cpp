#include "uuid.h"

#include <uuid/uuid.h>

posix::uuid_t::uuid_t() {
    ::uuid_t uuid={};
    uuid_generate(uuid);

    static const size_t UUID_STR_LENGTH = 16/*bytes*/ * 2/*nibbles*/ + 4/*hyphens*/ + 1/*null-terminator*/;
    char uuid_str[UUID_STR_LENGTH] = {};
    uuid_unparse(uuid, uuid_str);

    value = std::string(uuid_str);
}

std::string posix::uuid_t::str() const {
    return value;
}
