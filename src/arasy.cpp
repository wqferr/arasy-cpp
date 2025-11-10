#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;

int Lua::gettop() const {
    return lua_gettop(state);
}

void Lua::push(const LuaValue& value) {
    std::visit(
        [this](const auto& v) {
            v.pushOnto(this->state);
        },
        value
    );
}

LuaValue Lua::pop() {
    LuaValue ret = nil;

    if (gettop() != 0) {
        switch (lua_type(state, -1)) {
            case LUA_TNIL:
                // ret already contains nil
                break;

            case LUA_TNUMBER:
                if (lua_isinteger(state, -1)) {
                    ret.emplace<LuaInteger>(lua_tointeger(state, -1));
                } else {
                    ret.emplace<LuaNumber>(lua_tonumber(state, -1));
                }
                break;

            case LUA_TFUNCTION:
                break;
        }
        lua_pop(state, 1);
    }
    return ret;
}
