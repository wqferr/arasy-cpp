#include <gtest/gtest.h>
#include <memory>

#include "arasy.hpp"

#define SCOPE BasicLua

using namespace arasy::core;

const auto nimpl = testing::AssertionResult("Not implemented");

TEST(SCOPE, GetGlobal) {
    Lua L;
    ASSERT_EQ(luaL_dostring(L, "x = 1 + 2"), LUA_OK) << "Snippet didn't compile";

    {
        lua_getglobal(L, "x");
        int x = lua_tointeger(L, -1);
        ASSERT_EQ(x, 3) << "Global had unexpected value";
        lua_pop(L, 1);
    }

    {
        // TODO
        // FAIL(nimpl) << "TODO global variable proxy that allows you to set or get globals with C++ syntax";
    }
}

TEST(SCOPE, SetGlobal) {
    Lua L;
    int original = -4;
    {
        lua_pushinteger(L, original);
        lua_setglobal(L, "x");
        ASSERT_EQ(luaL_dostring(L, "out = 2*x"), LUA_OK) << "Snippet didn't compile";
        lua_getglobal(L, "out");
        int real = lua_tointeger(L, -1);
        ASSERT_EQ(real, 2*original) << "Global was not set properly";
        lua_pop(L, 1);
    }

    {
        // TODO
        // FAIL(nimpl) << "TODO global variable proxy that allows you to set or get globals with C++ syntax";
    }
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

    {
        ASSERT_EQ(arasy::nil, LuaNil{}) << "Not all nils are equal";
        L.push(arasy::nil);
        ASSERT_EQ(lua_gettop(L), 1) << "Number of pushed values was not exactly 1";
        ASSERT_TRUE(lua_isnil(L, -1)) << "Value pushed was not nil";
        lua_pop(L, 1);
    }

    {
        LuaNumber x = 5.0;
        ASSERT_FLOAT_EQ(x, 5.0) << "Type coersion from LuaNumber to lua_Number is not accurate";
        L.push(x);
        ASSERT_EQ(lua_gettop(L), 1) << "More than one value was pushed";
        ASSERT_TRUE(lua_isnumber(L, -1)) << "Value pushed was not a number";
        ASSERT_FLOAT_EQ(lua_tonumber(L, -1), x) << "Recovered value is different from value pushed";
        lua_pop(L, 1);
    }

    {
        LuaInteger i = 3;
        ASSERT_EQ(i, 3) << "Type coersion from LuaInteger to lua_Integer is not accurate";
        L.push(i);
        ASSERT_EQ(lua_gettop(L), 1) << "More than one value was pushed";
        ASSERT_TRUE(lua_isinteger(L, -1)) << "Value pushed was not an integer";
        ASSERT_EQ(lua_tointeger(L, -1), i) << "Recovered value is different from value pushed";
        lua_pop(L, 1);
    }

    {
        const char *rawstr = "abcdef";
        LuaString str {rawstr};
        ASSERT_EQ(str, rawstr) << "LuaString allocated memory; it should have made a shallow copy of the cstring";
        L.push(str);
        ASSERT_EQ(lua_gettop(L), 1) << "More than one value was pushed";
        ASSERT_TRUE(lua_isstring(L, -1)) << "Value pushed was not a string";
        ASSERT_STREQ(lua_tostring(L, -1), str.str) << "Recovered value is different from value pushed";
        lua_pop(L, 1);
    }
}

TEST(SCOPE, ArasyApiHasGetPop) {
    Lua L;

    L.pushStr("abc");
    L.pushInt(123);
    L.pushNil();
    L.pushNum(-0.5);

    EXPECT_TRUE(L.has<LuaNumber>(-1)) << "has<>() did not identify a number";
    EXPECT_TRUE(L.hasTop<LuaNumber>()) << "hasTop<>() did not index the stack correctly";
    EXPECT_FALSE(L.hasTop<LuaInteger>()) << "hasTop<>() identified a non-integer number as an integer";

    EXPECT_TRUE(L.has<LuaNil>(-2)) << "has<>() did not identify nil";
    std::optional<LuaValue> v = L.get<LuaNil>(-2);
    ASSERT_NE(v, std::nullopt) << "get<>() did not fetch a nil value";
    EXPECT_EQ(*v, LuaNil{}) << "get<>() fetched a non-nil value";
    EXPECT_TRUE(L.has<LuaNumber>(-3)) << "has<>() did not identify an integer as a number";
    EXPECT_TRUE(L.has<LuaInteger>(-3)) << "has<>() did not identify an integer";
    EXPECT_TRUE(L.has<LuaString>(-4)) << "has<>() did not identify a string";

    EXPECT_TRUE(L.has<LuaString>(1)) << "has<>() did not identify correctly with positive index";
    std::optional<LuaString> s = L.get<LuaString>(1);
    ASSERT_NE(s, std::nullopt) << "get<>() fetched a non-string value";
    EXPECT_STREQ(s->str, "abc") << "get<>() did not fetch correct string";

    EXPECT_TRUE(L.has<LuaNumber>(2)) << "has<>() did not identify an integer using positive indices";
    EXPECT_TRUE(L.has<LuaInteger>(2)) << "has<>() did not identify an integer using positive indices";
    auto i = L.get<LuaInteger>(2);
    EXPECT_NE(i, std::nullopt) << "get<>() did not fetch an integer value";
    EXPECT_EQ(*i, 123) << "get<>() did not fetch the correct integer";
    auto x = L.get<LuaNumber>(2);
    EXPECT_NE(x, std::nullopt) << "get<>() did not fetch a number value from an integer";
    EXPECT_EQ(*x, 123) << "get<>() did not fetch the correct number";
}

// TEST(SCOPE, LuaValueOstream) {
// }
