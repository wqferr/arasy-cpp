#include "arasy/types/thread.hpp"
#include "arasy.hpp"

using namespace arasy::core;

void LuaThread::pushOnto(lua_State* L) const {
    (void) L;
    lua_pushthread(thread_->state);
}

bool arasy::core::operator==(const LuaThread& a, const LuaThread& b) {
    return a.thread().state == b.thread().state;
}
