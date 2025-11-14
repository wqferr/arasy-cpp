#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

LuaRegistry::LuaRegistry(lua_State* L_): L(L_) {}

void LuaRegistry::retrieveField(const char* fieldName) const {
    lua_getfield(L, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::retrieveKey(const LuaValue& key) const {
    key.pushOnto(L);
    lua_gettable(L, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveStack() const {
    lua_gettable(L, LUA_REGISTRYINDEX);
}

void LuaRegistry::writeField(const char *fieldName, const LuaValue& value) {
    value.pushOnto(L);
    lua_setfield(L, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::storeField(const char* fieldName) {
    if (lua_gettop(L) < 1) {
        return;
    }

    lua_setfield(L, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::writeKey(const LuaValue& key, const LuaValue& value) {
    if (key.isA<LuaInteger>()) {
        return;
    }
    key.pushOnto(L);
    value.pushOnto(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

void LuaRegistry::storeKey(const LuaValue& key) {
    if (key.isA<LuaInteger>() || lua_gettop(L) < 1) {
        return;
    }

    key.pushOnto(L);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_pop(L, 2);
}

int LuaRegistry::createRef(int idx) {
    if (lua_isnone(L, idx)) {
        return LUA_NOREF;
    }
    lua_pushvalue(L, idx);
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveRef(int ref) const {
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
}

void LuaRegistry::releaseRef(int ref) {
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}
