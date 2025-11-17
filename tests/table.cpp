#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy;
using namespace arasy::core;
using namespace arasy::literals;

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

TEST(Table, CanSetValuesToNil) {
    Lua L;

    LuaTable t = L.createNewTable();
    t.setField("test", nil);
    auto v = t.getField("test");
    ASSERT_TRUE(v.has_value());
    EXPECT_TRUE(v->isNil());
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

    table.retrieveField("nah");
    EXPECT_EQ(L.stackSize(), 1);

    auto value = L.popStack<LuaBoolean>();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value, True);

    EXPECT_EQ(L.stackSize(), 0);
    table.set(True, 12);
    table.retrieve(True);
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
    EXPECT_EQ(err->code, arasy::error::IndexingErrorCode::NIL_KEY);

    EXPECT_EQ(L.stackSize(), 0);
    err.reset();
    err = table.retrieve(nil);
    EXPECT_EQ(L.stackSize(), 0);

    ASSERT_TRUE(err.has_value());
    EXPECT_EQ(err->code, arasy::error::IndexingErrorCode::NIL_KEY);

    err.reset();
    auto value = table.get(nil, err);
    ASSERT_TRUE(err.has_value());
    EXPECT_FALSE(value.has_value());
}

TEST(Table, CanSetFromStackValues) {
    Lua L;

    LuaTable table = L.createNewTable();
    L.pushStr("name");
    L.pushStr("William");
    table.setStackKV();

    EXPECT_EQ(*table.getField<LuaString>("name"), "William");
}

TEST(Table, CanGetFromStackKey) {
    Lua L;

    LuaTable table = L.createNewTable();
    table.setField("lastName", "Ferreira"_ls);

    L.pushStr("lastName");
    table.retrieveStackK();
    EXPECT_EQ(*L.popStack<LuaString>(), "Ferreira");
}

TEST(Table, CanUseOperatorSqBrackets) {
    Lua L;

    LuaTable t = L.createNewTable();
    t[True] = 1;
    t[False] = 0_li;
    t["field"] = "exists";
    t["lol"] = nil;
    EXPECT_THROW(t[nil] = "lol", std::runtime_error) << "Table did not error when setting a nil key";
    auto v = t["field"].get<LuaString>();
    ASSERT_TRUE(v.has_value()) << "Table did not retrieve existing key/value pair";
    EXPECT_EQ(v, "exists") << "Table retrieved incorrect value";
    auto u = t["lol"];
    EXPECT_TRUE(u->isNil());
    auto w = t[True];
    ASSERT_FALSE(w->isNil());
    ASSERT_TRUE(w->isNumeric());
    L.push(w);
    EXPECT_TRUE(L.checkStackTop<LuaNumber>());
}
