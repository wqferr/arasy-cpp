#pragma once

#include "lua.hpp"
#include "types/base.hpp"

namespace arasy::core {
    class LuaInteger : public LuaBaseType {
        const lua_Integer value;

    public:
        constexpr LuaInteger(lua_Integer value_): value(value_) {}
        constexpr operator lua_Integer() const { return value; }
        void pushOnto(lua_State* L) const override { lua_pushinteger(L, value); };
    };
}
