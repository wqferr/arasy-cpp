#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() {
    pushOnto(registry.L);
}

LuaReference::LuaReference(lua_State* L, int idx):
    registry(L),
    refCount(std::make_shared<char>(0)),
    id(registerId(L, idx))
{}

LuaReference::~LuaReference() {
    if (refCount.use_count() == 1) {
        registry.releaseRef(id);
    }
}

int LuaReference::registerId(lua_State* L, int idx) {
    lua_pushvalue(L, idx);
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void LuaReference::pushOnto(lua_State* L) const {
    registry.retrieveRef(id);
}

// LuaReference& LuaReference::operator=(const LuaReference& other) {
//
// }
//
// LuaReference& LuaReference::operator=(LuaReference&& other) {
//
// }
//
// LuaReference::LuaReference(const LuaReference& other) {
//
// }
//
// LuaReference::LuaReference(LuaReference&& other) {
//
// }
//
