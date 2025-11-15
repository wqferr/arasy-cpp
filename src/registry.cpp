#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

LuaRegistry::LuaRegistry(lua_State* L_): lua(L_) {}

void LuaRegistry::retrieveField(const char* fieldName) const {
    lua_getfield(lua, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::retrieveKey(const LuaValue& key) const {
    key.pushOnto(lua);
    lua_gettable(lua, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveStack() const {
    lua_gettable(lua, LUA_REGISTRYINDEX);
}

void LuaRegistry::writeField(const char *fieldName, const LuaValue& value) {
    value.pushOnto(lua);
    lua_setfield(lua, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::storeField(const char* fieldName) {
    if (lua_gettop(lua) < 1) {
        return;
    }

    lua_setfield(lua, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::writeKey(const LuaValue& key, const LuaValue& value) {
    if (key.isA<LuaInteger>()) {
        return;
    }
    key.pushOnto(lua);
    value.pushOnto(lua);
    lua_settable(lua, LUA_REGISTRYINDEX);
}

void LuaRegistry::storeKey(const LuaValue& key) {
    if (key.isA<LuaInteger>() || lua_gettop(lua) < 1) {
        return;
    }

    key.pushOnto(lua);
    lua_pushvalue(lua, -2);
    lua_settable(lua, LUA_REGISTRYINDEX);
    lua_pop(lua, 2);
}

#include <cassert>

int LuaRegistry::createRef(int idx) {
    if (lua_isnone(lua, idx)) {
        return LUA_NOREF;
    }
    lua_pushvalue(lua, idx);
    assert(lua_istable(lua, -1));
    return luaL_ref(lua, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveRef(int ref) const {
    lua_rawgeti(lua, LUA_REGISTRYINDEX, ref);
    assert(lua_istable(lua, -1));
}

void LuaRegistry::releaseRef(int ref) {
    luaL_unref(lua, LUA_REGISTRYINDEX, ref);
}
