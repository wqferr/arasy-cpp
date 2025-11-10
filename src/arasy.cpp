#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;

struct Pusher {
    Lua& L;
    Pusher(Lua& L_): L(L_) {}

    void operator()(const LuaNil&) const {
        lua_pushnil(L);
    }

    void operator()(const bool& b) const {
        lua_pushboolean(L, b);
    }

    void operator()(const LuaInteger& i) const {
        lua_pushinteger(L, i);
    }

    void operator()(const LuaNumber& v) const {
        lua_pushnumber(L, v);
    }

    void operator()(const LuaString& s) const {
        lua_pushstring(L, s);
    }
};

void Lua::push(const LuaValue& value) {
    std::visit(
        Pusher{*this},
        value
    );
}
