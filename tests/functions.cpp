#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;

extern "C" {
    namespace {
        int addSub(lua_State* L) {
            lua_checkstack(L, 2);
            lua_Number a = luaL_checknumber(L, 1);
            lua_Number b = luaL_checknumber(L, 2);
            lua_pushnumber(L, a+b);
            lua_pushnumber(L, a-b);
            return 2;
        }
    }
}

TEST(CFunctions, CanBeCalledNatively) {
    Lua L;
    lua_pushcfunction(L, addSub);
    L.push(5_li);
    L.push(3_li);
    ASSERT_EQ(lua_pcall(L, 2, 2, 0), LUA_OK);
    EXPECT_EQ(L.popStack<LuaInteger>(), 2_li);
    EXPECT_EQ(L.popStack<LuaInteger>(), 8_li);
}

TEST(CFunctions, CanBeCalledThroughTheArasyApi) {
    Lua L;
    lua_pushcfunction(L, addSub);
    auto maybeCfunc = L.popStack<LuaCFunction>();
    ASSERT_TRUE(maybeCfunc.has_value());
    auto cfunc = *maybeCfunc;
    auto err = cfunc.pcall(5_li, 3_li);
    EXPECT_FALSE(err.has_value());
    EXPECT_EQ(L.popStack<LuaInteger>(), 2_li);
    EXPECT_EQ(L.popStack<LuaInteger>(), 8_li);
}

TEST(CFunctions, CanPropagateErrorsThroughPcall) {
    Lua L;
    lua_pushcfunction(L, addSub);
    auto maybeCfunc = L.popStack<LuaCFunction>();
    ASSERT_TRUE(maybeCfunc.has_value());
    auto cfunc = *maybeCfunc;

    ASSERT_EQ(L.stackSize(), 0);
    auto err = cfunc.pcall(5_li);
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err, "bad argument #2 to '?' (number expected, got no value)");
    EXPECT_EQ(L.stackSize(), 0);
}

TEST(CFunctions, CanTruncateReturnValues) {
    Lua L;
    lua_pushcfunction(L, addSub);
    auto maybeCfunc = L.popStack<LuaCFunction>();
    ASSERT_TRUE(maybeCfunc.has_value());
    auto cfunc = *maybeCfunc;

    auto err = cfunc.pcall<1>(10_li, 5_li);
    EXPECT_FALSE(err.has_value());
    EXPECT_EQ(L.stackSize(), 1);
    EXPECT_EQ(L.popStack<LuaInteger>(), 15);
}
