#include "arasy/reference.hpp"
#include <stdexcept>

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() const {
    pushOnto(registry.registryContext);
}

int LuaReference::clone(const LuaReference& ref) {
    if (registry.registryContext != ref.registry.registryContext) {
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
    new (&registry) LuaRegistry {other.registry.registryContext};
    id_ = clone(other);
    return *this;
}

LuaReference& LuaReference::operator=(LuaReference&& other) {
    new (&registry) LuaRegistry {other.registry.registryContext};
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
    if (L != registry.registryContext) {
        throw std::runtime_error("Mixed registry references from different Lua instances");
    }
    registry.retrieveRef(id_);
}

bool LuaReference::operator==(const LuaReference& other) const {
    if (registry.registryContext != other.registry.registryContext) {
        return false;
    }

    pushOnto(registry.registryContext);
    const void* ptrA = lua_topointer(registry.registryContext, -1);
    other.pushOnto(registry.registryContext);
    const void* ptrB = lua_topointer(registry.registryContext, -1);
    lua_pop(registry.registryContext, 2);
    return ptrA == ptrB;
}
