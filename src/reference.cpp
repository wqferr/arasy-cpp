#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushSelf() {
    pushOnto(registry.L);
}

LuaReference::LuaReference(lua_State* L, int idx):
    registry(L),
    refCount(std::make_shared<char>(0)),
    id(findOrRegisterId(L, idx))
{}

LuaReference::~LuaReference() {
    if (refCount.use_count() == 1) {
        registry.releaseRef(id);
    }
}

int LuaReference::findOrRegisterId(lua_State* L, int idx) {
    lua_pushvalue(L, idx);
    registry.retrieveStack();
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_pushvalue(L, idx);
        return luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        int ref = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return ref;
    }
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
