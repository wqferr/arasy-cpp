#pragma once

#include <variant>

#include "lua.hpp"
#include "types/base.hpp"
#include "types/all.hpp"

namespace arasy::core {
    class Lua {
    public:
        lua_State *const state;

        Lua(): state(luaL_newstate()) {}
        ~Lua() { lua_close(state); }

        int gettop() const;
        void push(const LuaValue& value);
        LuaValue pop();

        operator lua_State*() const { return state; }
    };
}
