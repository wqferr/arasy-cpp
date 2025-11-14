#include <gtest/gtest.h>

#include "arasy.hpp"

using namespace arasy::core;
using namespace arasy::registry;

TEST(LuaRegistry, CanWriteAndRead) {
    Lua L;
    L.pushInt(0);
    L.registry.writeField("f", LuaInteger{3});
    EXPECT_EQ(L.stackSize(), 1) << "Registry modified the stack beyond its encapsulation";
    L.registry.writeField("g", LuaString{"abc"});
    EXPECT_EQ(L.stackSize(), 1) << "Registry modified the stack beyond its encapsulation";
    L.pushTable();

    EXPECT_EQ(L.registry.readField<LuaInteger>("f"), LuaInteger{3}) << "Registry did not retrieve integer field";
    EXPECT_EQ(L.stackSize(), 2) << "Registry modified the stack beyond its encapsulation";
    EXPECT_EQ(L.registry.readField("g"), LuaString{"abc"}) << "Registry did not retrieve string field";
    EXPECT_EQ(L.stackSize(), 2) << "Registry modified the stack beyond its encapsulation";

    ASSERT_NE(L.registry.readField<LuaString>("g"), std::nullopt) << "Registry did not retrieve string value";
    EXPECT_EQ(*L.registry.readField<LuaString>("g"), "abc") << "Registry did not retrieve and cast string field";
    EXPECT_EQ(L.registry.readField<LuaInteger>("g"), std::nullopt) << "Registry did not return std::nullopt for an invalid read cast";
    EXPECT_EQ(L.registry.readField("Nonexistent key"), nil) << "Registry did not push a nil value on accessing nonexistent key";

    const void* tablePtr1 = lua_topointer(L, -1);
    L.registry.storeField("mytable");
    EXPECT_EQ(L.stackSize(), 1) << "Registry did not pop the value it just stored";
    L.registry.retrieveField("mytable");
    lua_gc(L, LUA_GCCOLLECT);
    const void* tablePtr2 = lua_topointer(L, -1);
    EXPECT_EQ(tablePtr1, tablePtr2) << "Retrieved table is not the same";
    lua_pop(L, 1);

    auto maybeTable = L.registry.readField<LuaTable>("mytable");
    ASSERT_NE(maybeTable, std::nullopt);
    std::cerr << maybeTable->id() << '\n';
    L.push(*maybeTable);
    auto maybeOtherRef = L.readStackTop<LuaTable>();
    ASSERT_NE(maybeOtherRef, std::nullopt);
    const void* tablePtr3 = lua_topointer(L, -1);
    EXPECT_EQ(tablePtr1, tablePtr3);
    EXPECT_EQ(maybeOtherRef->id(), maybeTable->id());
}
