#pragma once

#include "lua.hpp"

#include <memory>

namespace arasy::core {
    class LuaBaseType {
    public:
        virtual void pushOnto(lua_State* L) const = 0;
    };
}
