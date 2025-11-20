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
        auto r = L.resumeOther(true, co1);
        ASSERT_TRUE(r.isOk()) << "Error in co1 yield";
        ASSERT_FALSE(r.value().finished) << "Co1 finished prematurely";
        ASSERT_EQ(r.value().nret, 1);
        ASSERT_EQ(L.stackSize(), 1);
        auto maybeInt = L.popStack<LuaInteger>();
        ASSERT_TRUE(maybeInt.has_value()) << "Co1 did not yield a value";
        EXPECT_EQ(*maybeInt, expected) << "Co1 did not yield as expected";
    }
    auto r = L.resumeOther(true, co1);
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
        auto result = L.resumeOther(true, co2, vars[stage-1]);
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

namespace {
    int yielder2(lua_State* ls, int status, lua_KContext ctx) {
        Lua L {ls};
        L.ensureStack(1);
        L.setGlobal("Part3", *L.popStack());

        return L.yield(0);
    }

    int nestedYielder(lua_State* ls) {
        Lua L {ls};
        L.setGlobal("Part2", "a string"_ls); // (1)
        L.push("this is an arg"_ls); // (2) (return)
        return L.yieldk(1, 0, &yielder2);
    }

    void checkThreadResult(const ResumeResult& result, int nexp) {
        if (result.isError()) {
            throw std::runtime_error("Error returning from nestedYielder()");
        }
        if (result->nret != nexp) {
            throw std::runtime_error("Unexpected number of returns");
        }
    }

    int continueNestedThread(lua_State* ls, int status, lua_KContext ctx) {
        Lua L {ls};
        auto nestedThr = *L.popStack<LuaThread>();
        auto i = *L.popStack<LuaInteger>();
        if (i.value != 32) {
            throw std::runtime_error("Unexpected argument in resume call");
        }
        nestedThr.resume(true, L, 0_li);
        L["Part4"] = -1_ln;
        return L.yield(0);
    }

    int setupNestedThread(lua_State* ls) {
        Lua L {ls};
        auto thr = L.createNewThread();
        L.ensureStack(1);
        L["Part1"] = *L.popStack();
        auto result = thr.start(true, L, &nestedYielder); // start
        checkThreadResult(result, 1);
        EXPECT_EQ(*L.readStackTop(), "this is an arg"_lv);
        L.push(" CONCAT"_ls);
        L.concat(2);
        L.push(thr);
        return L.yieldk(2, 0, &continueNestedThread);
    }

    int oneShotYielder(lua_State* ls) {
        Lua L {ls};
        return L.yield(0);
    }
}

TEST(Thread, ResumingPastEndIsError) {
    Lua L;
    LuaThread thr = L.createNewThread();
    auto result = thr.start(false, L, &oneShotYielder);
    ASSERT_TRUE(result.isOk());
    EXPECT_FALSE(result->finished);

    result = thr.resume(false, L);
    ASSERT_TRUE(result.isOk());
    EXPECT_TRUE(result->finished);

    result = thr.resume(false, L);
    ASSERT_TRUE(result.isError());
}

TEST(Thread, CanYieldFromInsideNestedThread) {
    Lua L;
    luaL_openlibs(L);
    lua_atpanic(L, &atPanic);

    auto thr = L.createNewThread();
    int s = L.stackSize();
    EXPECT_TRUE(L["Part1"].value().isNil());
    EXPECT_TRUE(L["Part2"].value().isNil());
    EXPECT_TRUE(L["Part3"].value().isNil());
    EXPECT_TRUE(L["Part4"].value().isNil());

    auto result = thr.start(true, L, &setupNestedThread, 123_li);
    ASSERT_TRUE(result.isOk());
    EXPECT_EQ(L["Part1"].value(), 123_lv);
    EXPECT_EQ(L["Part2"].value(), "a string"_lv);
    EXPECT_TRUE(L["Part3"].value().isNil());
    EXPECT_TRUE(L["Part4"].value().isNil());

    EXPECT_EQ(result->nret, 2);
    ASSERT_EQ(L.type(-1), LuaValueVarIndex::LuaThread);
    ASSERT_TRUE(L.checkStackTop<LuaThread>());
    ASSERT_TRUE(L.checkStack<LuaString>(-2));
    auto nestedThread = *L.popStack<LuaThread>();
    EXPECT_EQ(*L.popStack<LuaString>(), "this is an arg CONCAT"_ls);
    result = thr.resume(true, L, 32_li, nestedThread);
    EXPECT_EQ(result->nret, 0);
    EXPECT_EQ(L["Part1"].value(), 123_lv);
    EXPECT_EQ(L["Part2"].value(), "a string"_lv);
    EXPECT_EQ(L["Part3"].value(), 0_lv);
    EXPECT_EQ(L["Part4"].value(), -1_lv);
}
