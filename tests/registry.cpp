#include <gtest/gtest.h>

#include "arasy.hpp"

using namespace arasy::core;
using namespace arasy::registry;

TEST(LuaRegistry, CanWriteAndRead) {
    Lua L;
    L.registry->writeField("f", LuaInteger{3});
    EXPECT_EQ(L.stackSize(), 0);
    L.registry->writeField("g", LuaString{"abc"});
    EXPECT_EQ(L.stackSize(), 0);
    L.pushTable();

    EXPECT_EQ(L.registry->readField("f"), LuaInteger{3}) << "Registry did not retrieve integer field";
    EXPECT_EQ(L.stackSize(), 1);
    EXPECT_EQ(L.registry->readField("g").asA<LuaString>(), "abc") << "Registry did not retrieve string field";
    EXPECT_EQ(L.stackSize(), 1);
}
