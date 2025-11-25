#include "arasy/types/indexable.hpp"
#include "arasy.hpp"
#include "lua.h"
#include "lua.hpp"

using namespace arasy;
using namespace arasy::core::internal;

using IndexedValue = LuaIndexable::IndexedValue;

error::MIndexingError LuaIndexable::setStackKV() {
    Lua L {registry.luaInstance};
    lua_checkstack(L, 2);
    LuaValue value = *L.popStack();
    LuaValue key = *L.popStack();
    return set(key, value);
}

error::MIndexingError LuaIndexable::set(
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

lua_Integer LuaIndexable::len() const {
    pushSelf();
    lua_len(registry.luaInstance, -1);
    lua_Integer res = lua_tointeger(registry.luaInstance, -1);
    lua_pop(registry.luaInstance, 1);
    return res;
}

error::MIndexingError LuaIndexable::setField(
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

error::MIndexingError LuaIndexable::setRawStackKV() {
    Lua L {registry.luaInstance};
    lua_checkstack(L, 2);
    LuaValue value = *L.popStack();
    LuaValue key = *L.popStack();
    return setRaw(key, value);
}

error::MIndexingError LuaIndexable::setRaw(
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

error::MIndexingError LuaIndexable::setRawi(
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

error::MIndexingError LuaIndexable::retrieve(const LuaValue& key) {
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

error::MIndexingError LuaIndexable::retrieveStackK() {
    Lua L {registry.luaInstance};
    L.ensureStack(1);
    auto key = *L.popStack();
    return retrieve(key);
}

error::MIndexingError LuaIndexable::retrieveRaw(const LuaValue& key) {
    if (key.isNil()) {
        return {arasy::error::IndexingErrorCode::NIL_KEY};
    }
    lua_State* L = registry.luaInstance;
    pushSelf();
    key.pushOnto(L);
    lua_rawget(L, -2);
    lua_remove(L, -2);
    return std::nullopt;
}

error::MIndexingError LuaIndexable::retrieveRawStackK() {
    Lua L {registry.luaInstance};
    L.ensureStack(1);
    auto key = *L.popStack();
    return retrieveRaw(key);
}

void LuaIndexable::retrieveField(const char* fieldName) {
    lua_State* L = registry.luaInstance;
    pushSelf();
    lua_getfield(L, -1, fieldName);
    lua_remove(L, -2);
}

void LuaIndexable::retrieveI(int i) {
    lua_State* L = registry.luaInstance;
    pushSelf();
    lua_geti(L, -1, i);
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

IndexedValue::IndexedValue(LuaIndexable& t_, const LuaValue& k):
    t(t_),
    key_(makeKey(k)),
    dummyForArrowOp(std::make_shared<LuaValue>(nil))
{}

std::shared_ptr<LuaValue> IndexedValue::makeKey(const LuaValue& k) noexcept(false) {
    if (k.isNil()) {
        throw std::runtime_error("Attempted to index a table or userdata with nil");
    }
    return std::make_shared<LuaValue>(k);
}

LuaValue IndexedValue::key() const {
    return *key_;
}

IndexedValue LuaIndexable::operator[](const LuaValue& k) {
    return {*this, k};
}

IndexedValue LuaIndexable::operator[](const char* k) {
    return {*this, LuaString{k}};
}

// IndexedValue LuaIndexable::operator[](const lua_Integer& k) {
//     return {*this, LuaInteger{k}};
// }

IndexedValue::operator LuaValue() const {
    return *get<LuaValue>();
}

IndexedValue& IndexedValue::operator=(const lua_Number& value) {
    return *this = LuaNumber{value};
}

IndexedValue& IndexedValue::operator=(const char* str) {
    return *this = LuaString{str};
}

void IndexedValue::set(const lua_Number& value) {
    *this = value;
}

void IndexedValue::set(const char* value) {
    *this = value;
}

const LuaValue& IndexedValue::operator*() const {
    get<LuaValue>()->visit(
        [this](auto&& v) {
            new (this->dummyForArrowOp.get()) LuaValue {v};
        }
    );
    return *dummyForArrowOp;
}

std::shared_ptr<const LuaValue> IndexedValue::operator->() const {
    get<LuaValue>()->visit(
        [this](auto&& v) {
            new (this->dummyForArrowOp.get()) LuaValue {v};
        }
    );
    return dummyForArrowOp;
}
