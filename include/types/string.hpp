#pragma once

namespace arasy::core {
    class LuaString {
        const char *str;

    public:
        LuaString(const char *str_): str(str_) {}
        operator const char *() const { return str; }
    };
}
