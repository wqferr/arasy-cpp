#pragma once

#include "lua.hpp"

namespace arasy::core {
    class LuaInteger {
        const lua_Integer value;

    public:
        LuaInteger(lua_Integer value_): value(value_) {}
        operator lua_Integer() const { return value; }
    };
}
