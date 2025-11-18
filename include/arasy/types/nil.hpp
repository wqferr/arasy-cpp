#pragma once

#include "arasy/types/base.hpp"

namespace arasy::core {
    struct LuaNil : public internal::LuaBaseType {
        void pushOnto(lua_State* L) const override { lua_pushnil(L); }
    };
    constexpr bool operator==(const LuaNil&, const LuaNil&) { return true; }

    constexpr const LuaNil nil {};

    namespace internal {
        template<>
        struct LuaStackReader<LuaNil> {
            static bool checkAt(lua_State* L, int idx) {
                return lua_isnil(L, idx);
            }

            static std::optional<LuaNil> readAt(lua_State* L, int idx) {
                (void) L;
                (void) idx;
                return nil;
            }
        };
    }
}
