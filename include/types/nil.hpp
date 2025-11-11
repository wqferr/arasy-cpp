#pragma once

#include "types/base.hpp"

namespace arasy::core {
    struct LuaNil : public internal::LuaBaseType {
        constexpr bool operator==(const LuaNil& other) const { return true; }

        void pushOnto(lua_State* L) const override { lua_pushnil(L); }
    };

    constexpr const LuaNil nil {};

    namespace internal {
        template<>
        struct LuaStackReader<LuaNil> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isnil(L, idx);
            }

            static std::optional<LuaNil> readAt(lua_State* L, int idx) {
                return nil;
            }
        };
    }
}

namespace arasy {
    constexpr const inline core::LuaNil nil = core::nil;
}
