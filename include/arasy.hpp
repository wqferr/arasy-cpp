#pragma once

#include <variant>

#include "lua.hpp"
#include "types/all.hpp"

namespace arasy::core {
    using LuaValue = std::variant<LuaNil, LuaBoolean, LuaInteger, LuaNumber, LuaString>;

    class Lua {
    public:
        lua_State *const state;

        Lua(): state(luaL_newstate()) {}
        ~Lua() { lua_close(state); }

        void push(const LuaValue& value);
        LuaValue pop();

        operator lua_State*() const { return state; }
    };
}
