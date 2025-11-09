#include <gtest/gtest.h>
#include <memory>

#include "arasy.hpp"

using namespace arasy::core;

TEST(BasicLua, GetGlobal) {
    Lua L;
    ASSERT_EQ(luaL_dostring(L, "x = 1 + 2"), LUA_OK);
    lua_getglobal(L, "x");
    int x = lua_tointeger(L, -1);
    ASSERT_EQ(x, 3) << "Lua code executed properly";
}
