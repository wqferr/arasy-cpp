#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() const {
    pushOnto(registry.lua);
}

int LuaReference::clone(const LuaReference& ref) {
    if (registry.lua != ref.registry.lua) {
        return LUA_NOREF;
    }
    ref.pushSelf();
    return registry.createRef(-1);
}

LuaReference::LuaReference(lua_State* L, int idx):
    registry(L),
    id_(makeId(idx))
{}

LuaReference::LuaReference(const LuaReference& other):
    registry(other.registry),
    id_(clone(other))
{}

LuaReference::LuaReference(LuaReference&& other):
    registry(std::move(other.registry)),
    id_(std::move(other.id_))
{
    other.id_ = LUA_REFNIL;
}

LuaReference& LuaReference::operator=(const LuaReference& other) {
    new (&registry) LuaRegistry {other.registry.lua};
    id_ = clone(other);
    return *this;
}

LuaReference& LuaReference::operator=(LuaReference&& other) {
    new (&registry) LuaRegistry {other.registry.lua};
    id_ = std::move(other.id_);
    other.id_ = LUA_REFNIL;
    return *this;
}

int LuaReference::makeId(int idx) {
    return registry.createRef(idx);
}

LuaReference::~LuaReference() {
    registry.releaseRef(id_);
}

void LuaReference::pushOnto(lua_State* L) const {
    registry.retrieveRef(id_);
}

bool LuaReference::operator==(const LuaReference& other) const {
    if (registry.lua != other.registry.lua) {
        return false;
    }

    pushOnto(registry.lua);
    const void* ptrA = lua_topointer(registry.lua, -1);
    other.pushOnto(registry.lua);
    const void* ptrB = lua_topointer(registry.lua, -1);
    lua_pop(registry.lua, 2);
    return ptrA == ptrB;
}
