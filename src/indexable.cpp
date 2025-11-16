#include "arasy/types/indexable.hpp"
#include "arasy.hpp"
#include "lua.hpp"

using namespace arasy::core;
using namespace arasy::core::internal;

std::optional<arasy::error::IndexingError> LuaIndexable::setStackKV() {
    Lua L {registry.luaInstance};
    lua_checkstack(L, 2);
    LuaValue value = *L.popStack();
    LuaValue key = *L.popStack();
    return set(key, value);
}

std::optional<arasy::error::IndexingError> LuaIndexable::set(
    const LuaValue& key,
    const LuaValue& value
) {
    if (key.isNil()) {
        return {arasy::error::IndexingErrorCode::NIL_KEY};
    }
    lua_State* L = registry.luaInstance;
    pushSelf();
    key.pushOnto(L);
    value.pushOnto(L);
    lua_settable(L, -3);
    lua_pop(L, 1);
    return std::nullopt;
}

std::optional<arasy::error::IndexingError> LuaIndexable::setField(
    const char* fieldName,
    const LuaValue& value
) {
    lua_State* L = registry.luaInstance;

    pushSelf();
    value.pushOnto(L);
    lua_setfield(L, -2, fieldName);
    lua_pop(L, 1);
    return std::nullopt;
}

std::optional<arasy::error::IndexingError> LuaIndexable::setRawStackKV() {
    Lua L {registry.luaInstance};
    lua_checkstack(L, 2);
    LuaValue value = *L.popStack();
    LuaValue key = *L.popStack();
    return setRaw(key, value);
}

std::optional<arasy::error::IndexingError> LuaIndexable::setRaw(
    const LuaValue& key,
    const LuaValue& value
) {
    if (key.isNil()) {
        return {arasy::error::IndexingErrorCode::NIL_KEY};
    }
    lua_State* L = registry.luaInstance;
    pushSelf();
    key.pushOnto(L);
    value.pushOnto(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
    return std::nullopt;
}

std::optional<arasy::error::IndexingError> LuaIndexable::setRawi(
    const LuaInteger& key,
    const LuaValue& value
) {
    lua_State* L = registry.luaInstance;
    pushSelf();
    value.pushOnto(L);
    lua_rawseti(L, -2, key.value);
    lua_pop(L, 1);
    return std::nullopt;
}

std::optional<arasy::error::IndexingError> LuaIndexable::retrieve(const LuaValue& key) {
    if (key.isNil()) {
        return {arasy::error::IndexingErrorCode::NIL_KEY};
    }
    lua_State* L = registry.luaInstance;
    pushSelf();
    key.pushOnto(L);
    lua_gettable(L, -2);
    lua_remove(L, -2);
    return std::nullopt;
}

void LuaIndexable::retrieveField(const char* fieldName) {
    lua_State* L = registry.luaInstance;
    pushSelf();
    lua_getfield(L, -1, fieldName);
    lua_remove(L, -2);
}

void LuaIndexable::setMetatable(const LuaTable& mt) {
    pushSelf();
    mt.pushSelf();
    lua_setmetatable(registry.luaInstance, -2);
    lua_pop(registry.luaInstance, 1);
}

void LuaIndexable::setMetatableStack() {
    pushSelf();
    lua_rotate(registry.luaInstance, -2, 1);
    lua_setmetatable(registry.luaInstance, -2);
    lua_pop(registry.luaInstance, 2);
}
