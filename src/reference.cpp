#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() {
    pushOnto(registry.L);
}

LuaReference::LuaReference(lua_State* L, int idx):
    registry(L),
    id_(makeId(idx))
{}

int LuaReference::makeId(int idx) {
    lua_pushvalue(registry.L, idx);
    return registry.newRef();
}

LuaReference::~LuaReference() {
    if (refCount() == 1) {
        registry.releaseRef(id_);
    }
}

int LuaReference::refCount() {
    // TODO
    return 10;
}

void LuaReference::pushOnto(lua_State* L) const {
    registry.retrieveRef(id_);
}
