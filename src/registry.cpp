#include "arasy/registry.hpp"
#include "arasy.hpp"

using namespace arasy::registry;
using namespace arasy::core;

void LuaRegistry::pushSelf() {
    lua_pushlightuserdata(L, mainKey);
    lua_gettable(L, LUA_REGISTRYINDEX);
}

LuaValue LuaRegistry::getField(const char *fieldName) {
    pushSelf();
    lua_getfield(L, -1, fieldName);
    return *L.readStackTop();
}

LuaValue LuaRegistry::getIndex(const LuaValue& key) {
    pushSelf();
    L.push(key);
    lua_gettable(L, -2);
    return *L.readStackTop();
}

void LuaRegistry::setField(const char *fieldName, const LuaValue& value) {
    pushSelf();
    L.push(value);
    lua_setfield(L, -2, fieldName);
}

void LuaRegistry::setIndex(const LuaValue& key, const LuaValue& value) {
    pushSelf();
    L.push(key);
    L.push(value);
    lua_settable(L, -3);
}
