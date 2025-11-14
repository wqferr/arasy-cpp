#include <gtest/gtest.h>

#include "arasy.hpp"

using namespace arasy::core;
using namespace arasy::registry;

TEST(LuaRegistry, CanWriteAndRead) {
    Lua L;
    L.pushInt(0);
    L.registry().writeField("f", LuaInteger{3});
    EXPECT_EQ(L.stackSize(), 1) << "Registry modified the stack beyond its encapsulation";
    L.registry().writeField("g", LuaString{"abc"});
    EXPECT_EQ(L.stackSize(), 1) << "Registry modified the stack beyond its encapsulation";
    L.pushTable();

    EXPECT_EQ(L.registry().readField("f"), LuaInteger{3}) << "Registry did not retrieve integer field";
    EXPECT_EQ(L.stackSize(), 2) << "Registry modified the stack beyond its encapsulation";
    EXPECT_EQ(L.registry().readField("g"), LuaString{"abc"}) << "Registry did not retrieve string field";
    EXPECT_EQ(L.stackSize(), 2) << "Registry modified the stack beyond its encapsulation";

    const void *tablePtr1 = lua_topointer(L, -1);
    L.registry().storeField("mytable");
    EXPECT_EQ(L.stackSize(), 1) << "Registry did not pop the value it just stored";
    L.registry().retrieveField("mytable");
    lua_gc(L, LUA_GCCOLLECT);
    const void *tablePtr2 = lua_topointer(L, -1);
    EXPECT_EQ(tablePtr1, tablePtr2) << "Retrieved table is not the same";
}
