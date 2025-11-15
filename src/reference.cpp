#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() {
    pushOnto(registry.L);
}

LuaReference::LuaReference(lua_State* L, int idx):
    registry(L),
    refCounter(std::make_shared<char>('\0')),
    id_(makeId(idx))
{}

int LuaReference::makeId(int idx) {
    return registry.createRef(idx);
}

LuaReference::~LuaReference() {
    if (refCounter.use_count() == 1) {
        registry.releaseRef(id_);
    }
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
