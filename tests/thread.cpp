#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy;

namespace {
    int atPanic(lua_State* L) {
        throw std::runtime_error("Unexpected Lua panic");
    }
}

TEST(Thread, CanYieldUsingTheArasyApi) {
    Lua L;
    lua_atpanic(L, atPanic);
    luaL_openlibs(L);

    auto err = L.executeFile("tests/scripts/coroutines.lua");
    ASSERT_FALSE(err.has_value()) << "Error loading script: " << err.value();

    auto maybeCo1 = L.getGlobal<LuaThread>("Co1");
    ASSERT_TRUE(maybeCo1.has_value()) << "Failed to get threads";

    auto co1 = *maybeCo1;
    for (int expected = 1; expected <= 5; expected++) {
        auto r = L.resume(true, co1);
        ASSERT_TRUE(r.isOk()) << "Error in co1 yield";
        ASSERT_FALSE(r.value().finished) << "Co1 finished prematurely";
        ASSERT_EQ(r.value().nret, 1);
        ASSERT_EQ(L.stackSize(), 1);
        auto maybeInt = L.popStack<LuaInteger>();
        ASSERT_TRUE(maybeInt.has_value()) << "Co1 did not yield a value";
        EXPECT_EQ(*maybeInt, expected) << "Co1 did not yield as expected";
    }
    auto r = L.resume(true, co1);
    ASSERT_TRUE(r.isOk()) << "Error in co1 yield";
    EXPECT_TRUE(r.value().finished) << "Co1 did not finish when expected";
    ASSERT_EQ(r.value().nret, 2);
    EXPECT_EQ(L.popStack<LuaString>(), "def");
    EXPECT_EQ(L.popStack<LuaString>(), "abc");
}
