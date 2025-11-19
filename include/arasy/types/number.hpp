#pragma once

#include "arasy/lua.hpp"

namespace arasy::core {
    class LuaNumber : public internal::LuaBaseType {
    public:
        lua_Number value;

        constexpr LuaNumber(lua_Number value_): value(value_) {}
        void pushOnto(lua_State* L) const override { lua_pushnumber(L, value); }

        LuaNumber operator-() const {
            return -value;
        }
    };

    constexpr bool operator==(const LuaNumber& a, const LuaNumber& b) {
        return a.value == b.value;
    }

    constexpr inline bool operator==(const lua_Number& a, const LuaNumber& b) {
        return a == b.value;
    }

    constexpr inline bool operator==(const LuaNumber& a, const lua_Number& b) {
        return a.value == b;
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

namespace arasy::literals {
    inline core::LuaNumber operator"" _ln(long double value) {
        return core::LuaNumber{static_cast<lua_Number>(value)};
    }

    inline core::LuaNumber operator"" _ln(unsigned long long int value) {
        return core::LuaNumber{static_cast<lua_Number>(value)};
    }
}
