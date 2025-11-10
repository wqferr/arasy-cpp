#pragma once

#include "lua.hpp"

namespace arasy::core {
    class LuaNumber {
        const lua_Number value;

    public:
        constexpr LuaNumber(lua_Number value_): value(value_) {}
        constexpr operator lua_Number() const { return value; }
    };
}
