#include "arasy/registry/reference.hpp"
#include <stdexcept>

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::changeOwnershipTo(lua_State* L) {
    new (&registry) LuaRegistry {L};
    // id stays the same, all threads share a common registry
}

void LuaReference::doRegister() {
    id_ = makeId(-1);
    lua_pop(registry.luaInstance, 1);
}

void LuaReference::pushSelf() const {
    pushOnto(registry.luaInstance);
}

bool LuaReference::fromSameThreadTreeAs(lua_State* otherLua) const {
    lua_State* thisLua = registry.luaInstance;
    lua_rawgeti(thisLua, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
    const void* mainThreadOfThis = lua_topointer(thisLua, -1);
    lua_pop(thisLua, 1);

    lua_rawgeti(otherLua, LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
    const void* mainThreadOfOther = lua_topointer(otherLua, -1);
    lua_pop(otherLua, 1);

    return mainThreadOfThis == mainThreadOfOther;
}

int LuaReference::clone(const LuaReference& ref) {
    if (!fromSameThreadTreeAs(ref.registry.luaInstance)) {
        return LUA_NOREF;
    }
    ref.pushSelf();
    return luaL_ref(registry.luaInstance, LUA_REGISTRYINDEX);
}

LuaReference::LuaReference(lua_State* L):
    registry(L),
    id_(LUA_NOREF)
{}

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
    new (&registry) LuaRegistry {other.registry.luaInstance};
    id_ = clone(other);
    return *this;
}

LuaReference& LuaReference::operator=(LuaReference&& other) {
    new (&registry) LuaRegistry {std::move(other.registry.luaInstance)};
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
    if (!fromSameThreadTreeAs(L)) {
        throw std::runtime_error("Mixed registry references from different Lua instances");
    }
    registry.retrieveRef(id_);
}

bool LuaReference::operator==(const LuaReference& other) const {
    if (!fromSameThreadTreeAs(other.registry.luaInstance)) {
        return false;
    }

    pushOnto(registry.luaInstance);
    const void* ptrA = lua_topointer(registry.luaInstance, -1);
    other.pushOnto(registry.luaInstance);
    const void* ptrB = lua_topointer(registry.luaInstance, -1);
    lua_pop(registry.luaInstance, 2);
    return ptrA == ptrB;
}
