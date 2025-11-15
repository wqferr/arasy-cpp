#include "arasy/types/table.hpp"
#include "arasy/types/all.hpp"
#include "arasy.hpp"
#include "lua.hpp"

using namespace arasy::core;

std::optional<arasy::error::TableIndexingError> LuaTable::set(
    const LuaValue& key,
    const LuaValue& value
) {
    if (key.isNil()) {
        return {arasy::error::TableIndexingErrorCode::NIL_KEY};
    }
    lua_State* L = registry.lua;
    pushSelf();
    key.pushOnto(L);
    value.pushOnto(L);
    lua_settable(L, -3);
    lua_pop(L, 1);
    return std::nullopt;
}

std::optional<arasy::error::TableIndexingError> LuaTable::setField(
    const char* fieldName,
    const LuaValue& value
) {
    // TODO assert field name not null
    lua_State* L = registry.lua;

    pushSelf();
    value.pushOnto(L);
    lua_setfield(L, -2, fieldName);
    lua_pop(L, 1);
    return std::nullopt;
}

std::optional<arasy::error::TableIndexingError> LuaTable::index(const LuaValue& key) {
    if (key.isNil()) {
        return {arasy::error::TableIndexingErrorCode::NIL_KEY};
    }
    lua_State* L = registry.lua;
    pushSelf();
    key.pushOnto(L);
    lua_gettable(L, -2);
    lua_remove(L, -2);
    return std::nullopt;
}

void LuaTable::indexField(const char* fieldName) {
    lua_State* L = registry.lua;
    pushSelf();
    lua_getfield(L, -1, fieldName);
    lua_remove(L, -2);
}
