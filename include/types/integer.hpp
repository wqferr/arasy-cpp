#pragma once

#include "lua.hpp"

namespace arasy::core {
    class LuaInteger {
        const lua_Integer value;

    public:
        constexpr LuaInteger(lua_Integer value_): value(value_) {}
        constexpr operator lua_Integer() const { return value; }
    };
}
