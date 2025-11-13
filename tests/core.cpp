#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy::core;

TEST(BasicLua, GetGlobal) {
    Lua L;
    ASSERT_EQ(luaL_dostring(L, "x = 1 + 2"), LUA_OK) << "Snippet didn't compile";

    {
        auto val = L["x"].value();
        ASSERT_TRUE(std::holds_alternative<LuaInteger>(val)) << "Global variable indexing did not return an integer";
        ASSERT_EQ(std::readStack<LuaInteger>(val), 3) << "Global had unexpected value";
        EXPECT_EQ(L.stackSize(), 0) << "Extra values pushed onto the stack";
    }
}

TEST(BasicLua, SetGlobal) {
    Lua L;
    int a = -4;
    int b = 2;
    {
        L["x"] = a;
        L.setGlobalInt("y", b);
        ASSERT_EQ(luaL_dostring(L, "out = x*y"), LUA_OK) << "Snippet didn't compile";
        auto result = L["out"].value();
        EXPECT_TRUE(result.isA<LuaNumber>()) << "Global was not the correct type";
        ASSERT_EQ(result, a*b) << "Global was not set properly";
        EXPECT_EQ(L.stackSize(), 0) << "Extra values pushed onto the stack";
    }
}

TEST(BasicLua, LoadFile) {
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

TEST(BasicLua, PushWrapperTypes) {
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
        ASSERT_FLOAT_EQ(x.value, 5.0) << "Type coersion from LuaNumber to lua_Number is not accurate";
        L.push(x);
        ASSERT_EQ(lua_gettop(L), 1) << "More than one value was pushed";
        ASSERT_TRUE(lua_isnumber(L, -1)) << "Value pushed was not a number";
        ASSERT_FLOAT_EQ(lua_tonumber(L, -1), x.value) << "Recovered value is different from value pushed";
        lua_pop(L, 1);
    }

    {
        LuaInteger i = 3;
        ASSERT_EQ(i.value, 3) << "Type coersion from LuaInteger to lua_Integer is not accurate";
        L.push(i);
        ASSERT_EQ(lua_gettop(L), 1) << "More than one value was pushed";
        ASSERT_TRUE(lua_isinteger(L, -1)) << "Value pushed was not an integer";
        ASSERT_EQ(lua_tointeger(L, -1), i.value) << "Recovered value is different from value pushed";
        lua_pop(L, 1);
    }

    {
        const char *rawstr = "abcdef";
        LuaString str {rawstr};
        ASSERT_EQ(str, rawstr) << "LuaString allocated memory; it should have made a shallow copy of the cstring";
        L.push(str);
        ASSERT_EQ(lua_gettop(L), 1) << "More than one value was pushed";
        ASSERT_TRUE(lua_isstring(L, -1)) << "Value pushed was not a string";
        ASSERT_STREQ(lua_tostring(L, -1), str.str()) << "Recovered value is different from value pushed";
        lua_pop(L, 1);
    }
}

TEST(BasicLua, ArasyApiHasGetPop) {
    Lua L;

    L.pushStr("abc");
    L.pushInt(123);
    L.pushNil();
    L.pushNum(-0.5);

    EXPECT_TRUE(L.checkStack<LuaNumber>(-1)) << "checkStack<>() did not identify a number with a negative index";
    EXPECT_TRUE(L.checkStackTop<LuaNumber>()) << "checkStackTop<>() did not index the stack correctly";
    EXPECT_FALSE(L.checkStackTop<LuaInteger>()) << "checkStackTop<>() identified a non-integer number as an integer";

    EXPECT_TRUE(L.checkStack<LuaNil>(-2)) << "checkStack<>() did not identify nil with a negative index";
    std::optional<LuaValue> v = L.readStack<LuaNil>(-2);
    ASSERT_NE(v, std::nullopt) << "readStack<>() did not fetch a nil value with a negative index";
    EXPECT_EQ(*v, LuaNil{}) << "readStack<>() fetched a non-nil value with a negative index";
    EXPECT_TRUE(L.checkStack<LuaNumber>(-3)) << "checkStack<>() did not identify an integer as a number with a negative index";
    EXPECT_TRUE(L.checkStack<LuaInteger>(-3)) << "checkStack<>() did not identify an integer with a negative index";
    EXPECT_TRUE(L.checkStack<LuaString>(-4)) << "checkStack<>() did not identify a string with a negative index";

    EXPECT_TRUE(L.checkStack<LuaString>(1)) << "checkStack<>() did not identify a string with a positive index";
    std::optional<LuaString> s = L.readStack<LuaString>(1);
    ASSERT_NE(s, std::nullopt) << "readStack<>() fetched a non-string value with a positive index";
    EXPECT_STREQ(s->str(), "abc") << "readStack<>() did not fetch correct string with a positive index";

    EXPECT_TRUE(L.checkStack<LuaNumber>(2)) << "checkStack<>() did not identify an integer with a positive index";
    EXPECT_TRUE(L.checkStack<LuaInteger>(2)) << "checkStack<>() did not identify an integer with a positive index";
    v = L.readStack<LuaInteger>(2);
    ASSERT_NE(v, std::nullopt) << "readStack<>() did not fetch an integer value with a positive index";
    EXPECT_EQ(*v, 123) << "readStack<>() did not fetch the correct integer with a positive index";
    v = L.readStack<LuaNumber>(2);
    ASSERT_NE(v, std::nullopt) << "readStack<>() did not fetch a number value from an integer with a positive index";
    EXPECT_EQ(*v, 123) << "readStack<>() did not fetch the correct number with a positive index";

    EXPECT_TRUE(L.checkStack<LuaNil>(3)) << "checkStack<>() did not identify a nil with a positive index";
    v = L.readStack<LuaNil>(3);
    ASSERT_NE(v, std::nullopt) << "readStack<>() did not fetch a nil value with a positive index";
    EXPECT_EQ(*v, LuaNil{}) << "readStack<>() fetched a non-nil value";

    EXPECT_TRUE(L.checkStack<LuaNumber>(4)) << "checkStack<>() did not identify a number with a positive index";
    v = L.readStack<LuaInteger>(4);
    EXPECT_EQ(v, std::nullopt) << "readStack<>() fetched an integer from a non-integer number";
    v = L.readStack<LuaNumber>(4);
    ASSERT_NE(v, std::nullopt) << "readStack<>() did not fetch a number with a positive index";
    EXPECT_EQ(*v, -0.5) << "readStack<>() fetched the wrong number with a positive index";
}
