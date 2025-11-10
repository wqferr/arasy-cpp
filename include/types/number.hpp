#pragma once

#include "lua.hpp"

namespace arasy::core {
    class LuaNumber : public internal::LuaBaseType {
        const lua_Number value;

    public:
        constexpr LuaNumber(lua_Number value_): value(value_) {}
        constexpr operator lua_Number() const { return value; }
        void pushOnto(lua_State* L) const override { lua_pushnumber(L, value); };
    };

    template<>
    struct LuaStackReader<LuaNumber> {
        static std::optional<LuaNumber> readAt(lua_State* L, int idx) {
            int valid;
            lua_Number num = lua_tonumberx(L, idx, &valid);
            if (valid) {
                return num;
            } else {
                return std::nullopt;
            }
        }
    };
}
