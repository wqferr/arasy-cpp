#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

LuaRegistry::LuaRegistry(lua_State* L_): L(L_) {
    lua_pushlightuserdata(L, mainKey);
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

LuaRegistry::~LuaRegistry() {
    lua_pushlightuserdata(L, mainKey);
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);
    delete mainKey;
}

void LuaRegistry::pushSelf() {
    lua_pushlightuserdata(L, mainKey);
    lua_gettable(L, LUA_REGISTRYINDEX);
}

void LuaRegistry::retrieveField(const char* fieldName) {
    pushSelf();
    lua_getfield(L, -1, fieldName);
    lua_remove(L, -2);
}

LuaValue LuaRegistry::readField(const char* fieldName) {
    retrieveField(fieldName);
    auto value = *arasy::core::internal::LuaStackReader<LuaValue>::readAt(L, -1);
    lua_pop(L, 1);
    return value;
}

void LuaRegistry::retrieve(const LuaValue& key) {
    pushSelf();
    key.pushOnto(L);
    lua_gettable(L, -2);

    // Yes, twice
    lua_remove(L, -2);
    lua_remove(L, -2);
}

LuaValue LuaRegistry::readKey(const LuaValue& key) {
    retrieve(key);
    auto value = *arasy::core::internal::LuaStackReader<LuaValue>::readAt(L, -1);
    lua_pop(L, 1);
    return value;
}

void LuaRegistry::writeField(const char *fieldName, const LuaValue& value) {
    pushSelf();
    value.pushOnto(L);
    lua_setfield(L, -2, fieldName);
    lua_pop(L, 1);
}

void LuaRegistry::storeField(const char* fieldName) {
    if (lua_gettop(L) < 1) {
        return;
    }

    pushSelf();
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, fieldName);
    lua_pop(L, 1);
}

void LuaRegistry::writeKey(const LuaValue& key, const LuaValue& value) {
    pushSelf();
    key.pushOnto(L);
    value.pushOnto(L);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

void LuaRegistry::storeKey(const LuaValue& key) {
    if (lua_gettop(L) < 1) {
        return;
    }

    pushSelf();
    key.pushOnto(L);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);
}
