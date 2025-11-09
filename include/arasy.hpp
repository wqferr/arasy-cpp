#pragma once

#include "lua.hpp"

namespace arasy::core {
    class Lua {
    public:
        lua_State *const state;

        Lua(): state(luaL_newstate()) {}
        ~Lua() { lua_close(state); }

        operator lua_State*() const { return state; }
    };
}
