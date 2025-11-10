#pragma once

#include "types/base.hpp"

#include <memory>

namespace arasy::core {
    struct LuaNil : public LuaBaseType {
        constexpr bool operator==(const LuaNil& other) const { return true; }

        void pushOnto(lua_State* L) const override { lua_pushnil(L); };
    };
}

namespace arasy {
    constexpr const inline core::LuaNil nil {};
    const inline std::shared_ptr<core::LuaNil> nilptr = std::make_shared<core::LuaNil>();
}
