#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() const {
    pushOnto(registry.L);
}

int LuaReference::clone(const LuaReference& ref) {
    if (registry.L != ref.registry.L) {
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
    new (&registry) LuaRegistry {other.registry.L};
    id_ = clone(other);
    return *this;
}

LuaReference& LuaReference::operator=(LuaReference&& other) {
    new (&registry) LuaRegistry {other.registry.L};
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
    if (registry.L != other.registry.L) {
        return false;
    }

    pushOnto(registry.L);
    const void* ptrA = lua_topointer(registry.L, -1);
    other.pushOnto(registry.L);
    const void* ptrB = lua_topointer(registry.L, -1);
    lua_pop(registry.L, 2);
    return ptrA == ptrB;
}
