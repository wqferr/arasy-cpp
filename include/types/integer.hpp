#pragma once

#include "lua.hpp"
#include "types/base.hpp"

namespace arasy::core {
    class LuaInteger : public internal::LuaBaseType {
    public:
        lua_Integer value;

        constexpr LuaInteger(lua_Integer value_): value(value_) {}
        constexpr operator lua_Integer() const { return value; }
        void pushOnto(lua_State* L) const override { lua_pushinteger(L, value); };
    };

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
