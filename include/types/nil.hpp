#pragma once

#include "types/base.hpp"

namespace arasy::core {
    struct LuaNil : public internal::LuaBaseType {
        constexpr bool operator==(const LuaNil& other) const { return true; }

        void pushOnto(lua_State* L) const override { lua_pushnil(L); };
    };

    constexpr const inline LuaNil nil {};

    template<>
    struct LuaStackReader<LuaNil> {
        static std::optional<LuaNil> readAt(lua_State* L, int idx) {
            return nil;
        }
    };
}

namespace arasy {
    constexpr const inline core::LuaNil nil = core::nil;
}
