#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

LuaRegistry::LuaRegistry(lua_State* L_): registryContext(L_) {}

void LuaRegistry::retrieveField(const char* fieldName) const {
    lua_getfield(registryContext, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::retrieveKey(const LuaValue& key) const {
    key.pushOnto(registryContext);
    lua_gettable(registryContext, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveStack() const {
    lua_gettable(registryContext, LUA_REGISTRYINDEX);
}

void LuaRegistry::writeField(const char *fieldName, const LuaValue& value) {
    value.pushOnto(registryContext);
    lua_setfield(registryContext, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::storeField(const char* fieldName) {
    if (lua_gettop(registryContext) < 1) {
        return;
    }

    lua_setfield(registryContext, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::writeKey(const LuaValue& key, const LuaValue& value) {
    if (key.isA<LuaInteger>()) {
        return;
    }
    key.pushOnto(registryContext);
    value.pushOnto(registryContext);
    lua_settable(registryContext, LUA_REGISTRYINDEX);
}

void LuaRegistry::storeKey(const LuaValue& key) {
    if (key.isA<LuaInteger>() || lua_gettop(registryContext) < 1) {
        return;
    }

    key.pushOnto(registryContext);
    lua_pushvalue(registryContext, -2);
    lua_settable(registryContext, LUA_REGISTRYINDEX);
    lua_pop(registryContext, 2);
}

#include <cassert>

int LuaRegistry::createRef(int idx) {
    if (lua_isnone(registryContext, idx)) {
        return LUA_NOREF;
    }
    lua_pushvalue(registryContext, idx);
    assert(lua_istable(registryContext, -1));
    return luaL_ref(registryContext, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveRef(int ref) const {
    lua_rawgeti(registryContext, LUA_REGISTRYINDEX, ref);
    assert(lua_istable(registryContext, -1));
}

void LuaRegistry::releaseRef(int ref) {
    luaL_unref(registryContext, LUA_REGISTRYINDEX, ref);
}
