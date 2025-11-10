#include <gtest/gtest.h>
#include <memory>

#include "arasy.hpp"

#define SCOPE BasicLua

using namespace arasy::core;

TEST(SCOPE, GetGlobal) {
    Lua L;
    ASSERT_EQ(luaL_dostring(L, "x = 1 + 2"), LUA_OK) << "Snippet didn't compile";
    lua_getglobal(L, "x");
    int x = lua_tointeger(L, -1);
    ASSERT_EQ(x, 3) << "Global had unexpected value";
}

TEST(SCOPE, SetGlobal) {
    Lua L;
    int original = -4;
    lua_pushinteger(L, original);
    lua_setglobal(L, "x");
    ASSERT_EQ(luaL_dostring(L, "out = 2*x"), LUA_OK) << "Snippet didn't compile";
    lua_getglobal(L, "out");
    int real = lua_tointeger(L, -1);
    ASSERT_EQ(real, 2*original) << "Global was not set properly";
}

TEST(SCOPE, LoadFile) {
    Lua L;
    luaL_openlibs(L);
    lua_newtable(L);
    lua_setglobal(L, "t");

    int status = luaL_loadfile(L, "tests/scripts/test_loading.lua");
    if (status == LUA_OK) {
        lua_getglobal(L, "t");
        lua_pushnumber(L, 0.5);
        status = lua_pcall(L, 2, 2, 0);
    }
    status = status && lua_pcall(L, 2, 2, 0);
    if (status != LUA_OK) {
        ASSERT_TRUE(lua_isstring(L, -1));
        FAIL() << lua_tostring(L, -1);
    }
    int validNumber = true;
    lua_Number f = lua_tonumberx(L, -1, &validNumber);
    ASSERT_TRUE(validNumber) << "top of the stack is not a valid number";
    ASSERT_FLOAT_EQ(f, 5) << "top of the stack is not 5";

    ASSERT_EQ(lua_type(L, -2), LUA_TTABLE) << "second from the top is not a table";
    lua_getfield(L, -2, "field");
    lua_Integer i = lua_tointegerx(L, -1, &validNumber);
    ASSERT_TRUE(validNumber) << "t.field is not an integer";
    ASSERT_EQ(i, 3) << "t.field is not 3";
}

TEST(SCOPE, PushWrapperTypes) {
    Lua L;
    LuaNumber x = 5.0;
    ASSERT_FLOAT_EQ(x, 5.0) << "Type conversion from LuaNumber to lua_Number is not accurate";
    L.push(x);
    ASSERT_TRUE(lua_isnumber(L, -1)) << "Value pushed was not a number";
    ASSERT_FLOAT_EQ(lua_tonumber(L, -1), x) << "Recovered value is different from value pushed";
}
