#pragma once

#include "lua.hpp"

namespace arasy::core {
    class LuaBaseType {
    public:
        virtual void pushOnto(lua_State* L) const = 0;
    };
}
