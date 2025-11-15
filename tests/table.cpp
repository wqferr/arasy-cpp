#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy::core;

TEST(Table, CanSetField) {
    Lua L;

    L.newTable();
    LuaTable table = *L.readStackTop<LuaTable>();
    EXPECT_EQ(L.stackSize(), 1);
    // TODO: allow passing regular strings
    table.setField("field name", "Hello World!");
    EXPECT_EQ(L.stackSize(), 1);

    L.pushStr("field name");
    lua_gettable(L, -2);
    EXPECT_EQ(L.stackSize(), 2);

    ASSERT_TRUE(L.checkStackTop<LuaString>());
    EXPECT_EQ(*L.readStackTop<LuaString>(), "Hello World!");
}


// get, ~getField~, set, ~setField~, index, indexField

TEST(Table, CanGetField) {
    Lua L;

    L.newTable();
    LuaTable table = *L.readStackTop<LuaTable>();
    // TODO: allow passing regular numbers
    table.setField("number", 42);
    EXPECT_EQ(L.stackSize(), 1);

    auto value = table.getField<LuaNumber>("number");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, 42);
}
