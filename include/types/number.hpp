#pragma once

#include "lua.hpp"

namespace arasy::core {
    class LuaNumber {
        const lua_Number value;

    public:
        LuaNumber(lua_Number value_): value(value_) {}
        operator lua_Number() const { return value; }
    };
}
