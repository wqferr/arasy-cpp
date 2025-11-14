#include "arasy/reference.hpp"

using namespace arasy::core;
using namespace arasy::registry;

void LuaReference::pushOnto(lua_State* L) const {
    registry.retrieveRef(id);
}
