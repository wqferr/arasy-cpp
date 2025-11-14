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
