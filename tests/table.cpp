#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;

TEST(Table, CanSet) {
    Lua L;

    L.pushNewTable();
    LuaTable table = *L.readStackTop<LuaTable>();
    EXPECT_EQ(L.stackSize(), 1);
    table.setField("field name", "Hello World!"_ls);
    EXPECT_EQ(L.stackSize(), 1);

    L.pushStr("field name");
    lua_gettable(L, -2);
    EXPECT_EQ(L.stackSize(), 2);

    ASSERT_TRUE(L.checkStackTop<LuaString>());
    EXPECT_EQ(*L.readStackTop<LuaString>(), "Hello World!");
}

TEST(Table, CanGet) {
    Lua L;

    L.pushNewTable();
    LuaTable table = *L.readStackTop<LuaTable>();
    table.setField("number", -42);
    EXPECT_EQ(L.stackSize(), 1);

    auto value = table.getField<LuaNumber>("number");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, -42);

    //test non string types
    table.set(2, "two"_ls);
    EXPECT_EQ(L.stackSize(), 1);
    auto value2 = table.get<LuaString>(2);
    EXPECT_EQ(value2, "two"_ls);

    table.set(True, "true"_ls);
    EXPECT_EQ(L.stackSize(), 1);
    auto value3 = table.get<LuaString>(True);
    EXPECT_EQ(value3, "true"_ls);

    table.set(False, "false"_ls);
    EXPECT_EQ(L.stackSize(), 1);
    auto value4 = table.get<LuaString>(False);
    EXPECT_EQ(value4, "false"_ls);
}

TEST(Table, CanIndex) {
    Lua L;

    L.pushNewTable();
    LuaTable table = *L.popStack<LuaTable>();
    EXPECT_EQ(L.stackSize(), 0);
    table.setField("nah", True);
    EXPECT_EQ(L.stackSize(), 0);
    table.setField("yah", False);
    EXPECT_EQ(L.stackSize(), 0);

    table.indexField("nah");
    EXPECT_EQ(L.stackSize(), 1);

    auto value = L.popStack<LuaBoolean>();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, True);

    EXPECT_EQ(L.stackSize(), 0);
    table.set(True, 12);
    table.index(True);
    EXPECT_EQ(L.stackSize(), 1);
    auto value2 = L.popStack<LuaInteger>();
    ASSERT_TRUE(value2.has_value());
    EXPECT_EQ(value2, 12);
}

TEST(Table, DetectsNilKeys) {
    Lua L;
    L.pushNewTable();
    LuaTable table = *L.popStack<LuaTable>();

    auto err = table.set(nil, 123);
    ASSERT_TRUE(err.has_value());
    EXPECT_EQ(err->code, arasy::error::TableIndexingErrorCode::NIL_KEY);

    EXPECT_EQ(L.stackSize(), 0);
    err.reset();
    err = table.index(nil);
    EXPECT_EQ(L.stackSize(), 0);

    ASSERT_TRUE(err.has_value());
    EXPECT_EQ(err->code, arasy::error::TableIndexingErrorCode::NIL_KEY);

    auto value = table.get(nil);
    EXPECT_FALSE(value.has_value());
}
