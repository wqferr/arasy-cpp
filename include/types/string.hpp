#pragma once

#include <cstring>

namespace arasy::core {
    class LuaString : public LuaBaseType {
    public:
        const char *str;
        constexpr LuaString(const char *str_): str(str_) {}
        void pushOnto(lua_State* L) const override { lua_pushstring(L, str); };

        bool operator==(const LuaString& other) const { return strcmp(str, other.str) == 0; }
    };
}
