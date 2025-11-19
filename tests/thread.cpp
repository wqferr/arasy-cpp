#include <gtest/gtest.h>
#include <array>
#include <vector>

#include "arasy.hpp"

using namespace arasy;
using namespace arasy::literals;

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

TEST(Thread, CanInfluenceGlobalScope) {
    using namespace std::string_literals;
    Lua L;

    lua_atpanic(L, &atPanic);
    luaL_openlibs(L);

    auto err = L.executeFile("tests/scripts/coroutines.lua");
    ASSERT_FALSE(err.has_value()) << "Error loading script: " << err.value();

    auto maybeCo2 = L.getGlobal<LuaThread>("Co2");
    ASSERT_TRUE(maybeCo2.has_value()) << "Failed to get threads";
    auto co2 = *maybeCo2;

    constexpr int niter = 3;
    std::array<LuaValue, niter> vars = {
        10_lv,
        "a string"_lv,
        False_lv
    };
    std::array<std::vector<LuaValue>, niter> returns {{
        {"a"_lv, 1_lv},
        {"b"_lv},
        {"c"_lv}
    }};

    auto checkVars = [&](int stage) {
        for (int i = 1; i <= stage; i++) {
            auto g = *L.getGlobal("Var"s + std::to_string(i));
            EXPECT_EQ(g, vars.at(i-1)) << "Var" << i << " was not set correctly";
        }
        for (int i = stage+1; i <= niter; i++) {
            auto g = *L.getGlobal("Var"s + std::to_string(i));
            EXPECT_TRUE(g.isNil()) << "Var" << i << " got set prematurely to " << g;
        }
    };
    auto checkReturns = [&](int stage, int nret) {
        ASSERT_EQ(nret, returns.at(stage-1).size()) << "Mismatch in number of yielded values";
        auto rets = L.multiPop(nret);
        EXPECT_EQ(rets, returns.at(stage-1)) << "Values yielded by coroutine do not match";
    };

    auto step = [&](int stage) {
        int s = L.stackSize();
        auto result = L.resume(true, co2, vars[stage-1]);
        ASSERT_TRUE(result.isOk()) << "Error during coroutine execution: " << result.error().message.value_or("<No message>");
        int newS = L.stackSize();
        EXPECT_EQ(result->nret, newS - s) << "Number of yielded values does not match difference in stack size";
        checkVars(stage);
        checkReturns(stage, result->nret);
    };

    checkVars(0);
    for (int i = 1; i <= niter; i++) {
        step(i);
    }
}

TEST(Thread, CannotResumePastEnd) {
    FAIL() << "Not implemented";
}

namespace {
    int yielder2(lua_State* ls, int status, lua_KContext ctx) {
        Lua L {ls};
        auto err = L.wrapScriptError(status);
        if (err.has_value()) {
            return err->forward(L);
        }
        L.setGlobal("Part2", "another string"_ls);
    }

    int yielder1(lua_State* ls) {

    }
}

TEST(Thread, CanCallFunctionsThatYield) {
    Lua L;
    luaL_openlibs(L);

    lua_newthread(L);
}
