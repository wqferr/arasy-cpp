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

        int testUpvalues(lua_State* L) {
            lua_checkstack(L, 1);
            lua_pushvalue(L, lua_upvalueindex(1));
            lua_Number up = luaL_checknumber(L, -1);
            lua_Number arg = luaL_checknumber(L, -2);
            lua_pushnumber(L, up+arg);
            return 1;
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
    ASSERT_EQ(L.stackSize(), 1);
    auto maybeCfunc = L.popStack<LuaCFunction>();
    ASSERT_EQ(L.stackSize(), 0);
    ASSERT_TRUE(maybeCfunc.has_value());
    auto cfunc = *maybeCfunc;

    ASSERT_EQ(L.stackSize(), 0);
    auto err = maybeCfunc->pcall(5_li);
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(*err, "bad argument #2 to '?' (number expected, got no value)");
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

TEST(CFunctions, CanUseUpvaluesNatively) {
    Lua L;
    lua_pushcfunction(L, testUpvalues);

    int upvalue = 3;
    int callValue = 5;

    lua_pushnumber(L, upvalue);
    lua_pushcclosure(L, testUpvalues, 1);
    EXPECT_NE(lua_topointer(L, -1), lua_topointer(L, -2)) << "CClosure and CFunction both share the same internal pointer";

    std::optional<LuaCFunction> maybeCf = L.popStack<LuaCFunction>();
    L.popStack();

    ASSERT_TRUE(maybeCf.has_value()) << "Could not retrieve a CFunction from the stack";
    auto cf = *maybeCf;
    ASSERT_EQ(L.stackSize(), 0) << "Unexpected values on top of stack";
    auto err = cf.pcall(callValue);
    ASSERT_FALSE(err.has_value()) << "Function errored unexpectedly";
    ASSERT_EQ(L.stackSize(), 1);
    auto maybeNum = L.popStack<LuaNumber>();
    ASSERT_TRUE(maybeNum.has_value()) << "Function did not return a number";
    EXPECT_EQ(*maybeNum, upvalue + callValue);

    auto copyOfCf = cf;
    int copyCallValue = -1;
    err = copyOfCf.pcall(copyCallValue);
    ASSERT_FALSE(err.has_value()) << "Fuction errored unexpectedly";
    ASSERT_EQ(L.stackSize(), 1) << "Function did not push expected number of return values";
    maybeNum = L.popStack<LuaNumber>();
    ASSERT_TRUE(maybeNum.has_value()) << "Function did not return a number";
    EXPECT_EQ(*maybeNum, upvalue + copyCallValue);
}

TEST(CFunctions, CanBeCreatedFromUpValuesInline) {
    Lua L;
    LuaInteger upvalue = 10;
    int callValue = 5;
    L.pushNum(0); // Sentinel

    LuaCFunction cf = L.createCClosureInlineUpvalues(&testUpvalues, upvalue);
    EXPECT_EQ(L.stackSize(), 1) << "Extra values were pushed or popped when creating the closure";
    auto err = cf.pcall(callValue);

    ASSERT_FALSE(err.has_value()) << "Fuction errored unexpectedly: " << *err;
    ASSERT_EQ(L.stackSize(), 2) << "Function did not push expected number of return values";
    auto maybeNum = L.popStack<LuaNumber>();
    ASSERT_TRUE(maybeNum.has_value()) << "Function did not return a number";
    EXPECT_EQ(*maybeNum, upvalue.value + callValue);
}

namespace {
    void loadNativeAddSub(Lua& L) {
        ASSERT_FALSE(L.executeString(
            "function addSub(a, b)\n"
            "   if not a then error('expected number for argument 1, got no value') end\n"
            "   if not b then error('expected number for argument 2, got no value') end\n"
            "   return a+b, a-b\n"
            "end"
        ).has_value()) << "Failed to load native addSub function";
    }
}

TEST(NativeFunctions, CanBeCalledThroughTheArasyApi) {
    Lua L;
    loadNativeAddSub(L);
    auto maybeNativeFunc = L.popStack<LuaFunction>();
    ASSERT_TRUE(maybeNativeFunc.has_value()) << "Failed to pop native function from the stack";
    auto func = *maybeNativeFunc;

    auto err = func.pcall(5_li, 3_li);
    EXPECT_FALSE(err.has_value()) << "Function errored unexpectedly";
    EXPECT_EQ(L.popStack<LuaInteger>(), 2_li) << "addSub() second return value was incorrect";
    EXPECT_EQ(L.popStack<LuaInteger>(), 8_li) << "addSub() first return value was incorrect";
    EXPECT_EQ(L.stackSize(), 0) << "addSub() pushed more values than expected";
}

TEST(NativeFunctions, CanPropagateErrorsThroughPcall) {
    Lua L;
    loadNativeAddSub(L);
    auto maybeNativeFunc = L.popStack<LuaFunction>();
    ASSERT_TRUE(maybeNativeFunc.has_value()) << "Failed to pop native function from the stack";
    auto func = *maybeNativeFunc;

    ASSERT_EQ(L.stackSize(), 0);
    auto err = func.pcall(5_li);
    EXPECT_TRUE(err.has_value()) << "Function did not error as expected";
    EXPECT_EQ(*err, "expected number for argument 2, got no value");
    EXPECT_EQ(L.stackSize(), 0) << "Function pushed values unexpectedly";
}

TEST(NativeFunctions, CanTruncateReturnValues) {
    Lua L;
    loadNativeAddSub(L);
    auto maybeNativeFunc = L.popStack<LuaFunction>();
    ASSERT_TRUE(maybeNativeFunc.has_value()) << "Failed to pop native function from the stack";
    auto func = *maybeNativeFunc;

    auto err = func.pcall<1>(10_li, 5_li);
    EXPECT_FALSE(err.has_value()) << "Function errored unexpectedly";
    EXPECT_EQ(L.stackSize(), 1) << "Function had a different number of returns than expected";
    EXPECT_EQ(L.popStack<LuaInteger>(), 15) << "Function did not return expected result";
}
