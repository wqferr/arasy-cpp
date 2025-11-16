#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

LuaRegistry::LuaRegistry(lua_State* L_): luaInstance(L_) {}

void LuaRegistry::retrieveField(const char* fieldName) const {
    lua_getfield(luaInstance, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::retrieveKey(const LuaValue& key) const {
    key.pushOnto(luaInstance);
    lua_gettable(luaInstance, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveStack() const {
    lua_gettable(luaInstance, LUA_REGISTRYINDEX);
}

void LuaRegistry::writeField(const char *fieldName, const LuaValue& value) {
    value.pushOnto(luaInstance);
    lua_setfield(luaInstance, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::storeField(const char* fieldName) {
    if (lua_gettop(luaInstance) < 1) {
        return;
    }

    lua_setfield(luaInstance, LUA_REGISTRYINDEX, fieldName);
}

void LuaRegistry::writeKey(const LuaValue& key, const LuaValue& value) {
    if (key.isA<LuaInteger>()) {
        return;
    }
    key.pushOnto(luaInstance);
    value.pushOnto(luaInstance);
    lua_settable(luaInstance, LUA_REGISTRYINDEX);
}

void LuaRegistry::storeKey(const LuaValue& key) {
    if (key.isA<LuaInteger>() || lua_gettop(luaInstance) < 1) {
        return;
    }

    key.pushOnto(luaInstance);
    lua_pushvalue(luaInstance, -2);
    lua_settable(luaInstance, LUA_REGISTRYINDEX);
    lua_pop(luaInstance, 2);
}

int LuaRegistry::createRef(int idx) {
    if (lua_isnone(luaInstance, idx)) {
        return LUA_NOREF;
    }
    lua_pushvalue(luaInstance, idx);
    return luaL_ref(luaInstance, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveRef(int ref) const {
    lua_rawgeti(luaInstance, LUA_REGISTRYINDEX, ref);
}

void LuaRegistry::releaseRef(int ref) {
    luaL_unref(luaInstance, LUA_REGISTRYINDEX, ref);
}
