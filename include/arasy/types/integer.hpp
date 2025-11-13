#pragma once

#include "arasy/lua.hpp"
#include "arasy/types/base.hpp"

namespace arasy::core {
    class LuaInteger : public internal::LuaBaseType {
    public:
        lua_Integer value;

        constexpr LuaInteger(lua_Integer value_): value(value_) {}
        void pushOnto(lua_State* L) const override { lua_pushinteger(L, value); };

        constexpr bool operator==(const LuaInteger& other) const {
            return value == other.value;
        }

        constexpr std::enable_if_t<!std::is_same_v<lua_Integer, int>, bool> operator==(const int& other) const {
            return value == other;
        }
    };

    constexpr inline bool operator==(const lua_Integer& a, const LuaInteger& b) {
        return a == b.value;
    }

    namespace internal {
        template<>
        struct LuaStackReader<LuaInteger> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isinteger(L, idx);
            }

            static std::optional<LuaInteger> readAt(lua_State* L, int idx) {
                int valid;
                lua_Integer num = lua_tointegerx(L, idx, &valid);
                if (valid) {
                    return num;
                } else {
                    return std::nullopt;
                }
            }
        };
    }
}
