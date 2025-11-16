#include "arasy/types/indexable.hpp"
#include "arasy.hpp"
#include "lua.hpp"

using namespace arasy::core::internal;

std::optional<arasy::error::IndexingError> LuaIndexable::set(
    const LuaValue& key,
    const LuaValue& value
) {
    if (key.isNil()) {
        return {arasy::error::TableIndexingErrorCode::NIL_KEY};
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

std::optional<arasy::error::IndexingError> LuaIndexable::retrieve(const LuaValue& key) {
    if (key.isNil()) {
        return {arasy::error::TableIndexingErrorCode::NIL_KEY};
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
