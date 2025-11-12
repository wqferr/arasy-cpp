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
        auto val = L["x"].value();
        ASSERT_TRUE(std::holds_alternative<LuaInteger>(val)) << "Global variable indexing did not return an integer";
        ASSERT_EQ(std::get<LuaInteger>(val), 3) << "Global had unexpected value";
        EXPECT_EQ(L.size(), 0) << "Extra values pushed onto the stack";
    }
}

TEST(SCOPE, SetGlobal) {
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
        EXPECT_EQ(L.size(), 0) << "Extra values pushed onto the stack";
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

    EXPECT_TRUE(L.has<LuaNumber>(-1)) << "has<>() did not identify a number with a negative index";
    EXPECT_TRUE(L.hasTop<LuaNumber>()) << "hasTop<>() did not index the stack correctly";
    EXPECT_FALSE(L.hasTop<LuaInteger>()) << "hasTop<>() identified a non-integer number as an integer";

    EXPECT_TRUE(L.has<LuaNil>(-2)) << "has<>() did not identify nil with a negative index";
    std::optional<LuaValue> v = L.get<LuaNil>(-2);
    ASSERT_NE(v, std::nullopt) << "get<>() did not fetch a nil value with a negative index";
    EXPECT_EQ(*v, LuaNil{}) << "get<>() fetched a non-nil value with a negative index";
    EXPECT_TRUE(L.has<LuaNumber>(-3)) << "has<>() did not identify an integer as a number with a negative index";
    EXPECT_TRUE(L.has<LuaInteger>(-3)) << "has<>() did not identify an integer with a negative index";
    EXPECT_TRUE(L.has<LuaString>(-4)) << "has<>() did not identify a string with a negative index";

    EXPECT_TRUE(L.has<LuaString>(1)) << "has<>() did not identify a string with a positive index";
    std::optional<LuaString> s = L.get<LuaString>(1);
    ASSERT_NE(s, std::nullopt) << "get<>() fetched a non-string value with a positive index";
    EXPECT_STREQ(s->str, "abc") << "get<>() did not fetch correct string with a positive index";

    EXPECT_TRUE(L.has<LuaNumber>(2)) << "has<>() did not identify an integer with a positive index";
    EXPECT_TRUE(L.has<LuaInteger>(2)) << "has<>() did not identify an integer with a positive index";
    v = L.get<LuaInteger>(2);
    ASSERT_NE(v, std::nullopt) << "get<>() did not fetch an integer value with a positive index";
    EXPECT_EQ(*v, 123) << "get<>() did not fetch the correct integer with a positive index";
    v = L.get<LuaNumber>(2);
    ASSERT_NE(v, std::nullopt) << "get<>() did not fetch a number value from an integer with a positive index";
    EXPECT_EQ(*v, 123) << "get<>() did not fetch the correct number with a positive index";

    EXPECT_TRUE(L.has<LuaNil>(3)) << "has<>() did not identify a nil with a positive index";
    v = L.get<LuaNil>(3);
    ASSERT_NE(v, std::nullopt) << "get<>() did not fetch a nil value with a positive index";
    EXPECT_EQ(*v, LuaNil{}) << "get<>() fetched a non-nil value";

    EXPECT_TRUE(L.has<LuaNumber>(4)) << "has<>() did not identify a number with a positive index";
    v = L.get<LuaInteger>(4);
    EXPECT_EQ(v, std::nullopt) << "get<>() fetched an integer from a non-integer number";
    v = L.get<LuaNumber>(4);
    ASSERT_NE(v, std::nullopt) << "get<>() did not fetch a number with a positive index";
    EXPECT_EQ(*v, -0.5) << "get<>() fetched the wrong number with a positive index";
}

#undef SCOPE
#define SCOPE PushFmt

TEST(SCOPE, DoesntMangleSimpleStrings) {
    using arasy::error::PushFmtError;
    Lua L;
    EXPECT_EQ(L.pushFmt("this is a simple string"), PushFmtError::NONE)
        << "pushFmt() expected arguments even in the absence of placeholders";
    auto res = L.getTop<LuaString>();
    ASSERT_NE(res, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*res, "this is a simple string") << "pushFmt() mangled a simple string with no arguments";
}

TEST(SCOPE, DoesntMangleCorrectlyNotatedArguments) {
    Lua L;
    EXPECT_EQ(L.pushFmt("literal then %s (%d) -> %f%%", "interpolated", 123, 0.5), arasy::error::PushFmtError::NONE)
        << "pushFmt() incorrectly identified errors in simple interpolation";
    auto str = L.getTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_STREQ(str->str, "literal then interpolated (123) -> 0.5%") << "pushFmt() mangled a simple formatting job";
}

TEST(SCOPE, UnderstandsAllIntegerFormats) {
    Lua L;
    EXPECT_EQ(L.pushFmt("%d %c", 100, 65), arasy::error::PushFmtError::NONE);
    auto str = L.getTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*str, "100 A");
}

// TODO test %g %G %e %E

// TEST(SCOPE, Threads) {
//     Lua mainThread;
// }
