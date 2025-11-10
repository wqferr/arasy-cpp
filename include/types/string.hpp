#pragma once

#include <cstring>

namespace arasy::core {
    class LuaString {
    public:
        const char *const str;
        constexpr LuaString(const char *str_): str(str_) {}
        bool operator==(const LuaString& other) const { return strcmp(str, other.str) == 0; }
    };
}
