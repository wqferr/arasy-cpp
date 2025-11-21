#include "arasy/module.hpp"
#include "arasy.hpp"

using namespace arasy;
using namespace arasy::utils;

using Field = Module::Field;

Module::Module(lua_State* L_, const char* name_):
    L(L_),
    name(name_),
    members(makeMembersTable(L_))
{}

LuaTable Module::makeMembersTable(lua_State* L_) {
    return Lua{L_}.createNewTable();
}

Field Module::field(const char* name_) {
    return {*this, name_};
}

int Module::ref() const {
    return ref_;
}

void Module::setRef(int r) {
    ref_ = r;
}

void Module::unref() {
    luaL_unref(L, LUA_REGISTRYINDEX, ref_);
    ref_ = LUA_NOREF;
}

LuaValue Field::value() const {
    return *parent.members.getField(name);
}

LuaValue Field::cachedValue() const {
    return cachedValue_;
}

void Field::set(const core::LuaValue& value) {
    parent.members.setField(name, value);
    cachedValue_ = value;
}

Field& Field::operator=(const core::LuaValue& value) {
    set(value);
    return *this;
}

const LuaValue* Field::operator->() const {
    cachedValue_ = value();
    return &cachedValue_;
}

Field::Field(Module& parent_, const char* name_): parent(parent_), name(name_) {}
