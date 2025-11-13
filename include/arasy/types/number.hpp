#pragma once

#include "arasy/lua.hpp"

namespace arasy::core {
    class LuaNumber : public internal::LuaBaseType {
    public:
        lua_Number value;

        constexpr LuaNumber(lua_Number value_): value(value_) {}
        void pushOnto(lua_State* L) const override { lua_pushnumber(L, value); }

        constexpr bool operator==(const LuaNumber& other) const {
            return value == other.value;
        }
    };

    constexpr bool operator==(const lua_Number& a, const LuaNumber& b) {
        return a == b.value;
    }

    namespace internal {
        template<>
        struct LuaStackReader<LuaNumber> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isnumber(L, idx);
            }

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
}
