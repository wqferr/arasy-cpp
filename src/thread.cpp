#include "arasy/types/thread.hpp"
#include "arasy.hpp"

using namespace arasy::core;

void LuaThread::pushOnto(lua_State* L) const {
    (void) L;
    lua_pushthread(thread->state);
}
