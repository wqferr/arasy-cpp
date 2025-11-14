#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

LuaRegistry::LuaRegistry(lua_State* L_): L(L_) {}

void LuaRegistry::retrieveField(const char* fieldName) {
    lua_getfield(L, LUA_REGISTRYINDEX, fieldName);
}

LuaValue LuaRegistry::readField(const char* fieldName) {
    retrieveField(fieldName);
    auto value = *arasy::core::internal::LuaStackReader<LuaValue>::readAt(L, -1);
    lua_pop(L, 1);
    return value;
}

void LuaRegistry::retrieve(const LuaValue& key) {
    key.pushOnto(L);
    lua_gettable(L, LUA_REGISTRYINDEX);
}

LuaValue LuaRegistry::readKey(const LuaValue& key) {
    retrieve(key);
    auto value = *arasy::core::internal::LuaStackReader<LuaValue>::readAt(L, -1);
    lua_pop(L, 1);
    return value;
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
    key.pushOnto(L);
    value.pushOnto(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

void LuaRegistry::storeKey(const LuaValue& key) {
    if (lua_gettop(L) < 1) {
        return;
    }

    key.pushOnto(L);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);
    lua_pop(L, 2);
}
