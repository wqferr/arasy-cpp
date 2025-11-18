#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy;

TEST(PushFmt, DoesntMangleSimpleStrings) {
    using arasy::error::PushFmtErrorCode;
    Lua L;
    EXPECT_EQ(L.pushFmt("this is a simple string"), no_error)
        << "pushFmt() expected arguments even in the absence of placeholders";
    auto str = L.readStackTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*str, "this is a simple string") << "pushFmt() mangled a simple string with no arguments";
}

TEST(PushFmt, DoesntMangleCorrectlyNotatedArguments) {
    Lua L;
    EXPECT_EQ(L.pushFmt("literal then %s (%d) -> %f%% %p", "interpolated", 123, 0.5, nullptr), no_error)
        << "pushFmt() incorrectly identified errors in simple interpolation";
    auto str = L.readStackTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*str, "literal then interpolated (123) -> 0.5% 0000000000000000") << "pushFmt() mangled a simple formatting job";
}

TEST(PushFmt, UnderstandsAllIntegerFormats) {
    Lua L;
    EXPECT_FALSE(L.pushFmt("%d %c", 100, 65).has_value());
    auto str = L.readStackTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*str, "100 A");
}

TEST(PushFmt, DetectsTooFewArguments) {
    Lua L;
    auto err = L.pushFmt("%d %d %d", 1, 2);
    EXPECT_TRUE(err.has_value() && err->code == arasy::error::PushFmtErrorCode::TOO_FEW_ARGS) << "pushFmt() did not detect lack of arguments for placeholders";

    EXPECT_TRUE(L.pushFmt("%f").matches(arasy::error::PushFmtErrorCode::TOO_FEW_ARGS)) << "pushFmt() did not detect lack of arguments for placeholders when there are no arguments";
    EXPECT_EQ(L.stackSize(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsTooManyArguments) {
    Lua L;
    auto err = L.pushFmt("%d", 1, 2);
    EXPECT_TRUE(err.has_value() && err->code == arasy::error::PushFmtErrorCode::TOO_MANY_ARGS) << "pushFmt() did not detect excess of arguments for placeholders";
    EXPECT_TRUE(L.pushFmt("string with no placeholders", "this should not be used").matches(error::PushFmtErrorCode::TOO_MANY_ARGS)) << "pushFmt() did not detect excess of arguments for placeholders when there are no placeholders";
    EXPECT_EQ(L.stackSize(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsInvalidSpecifiers) {
    Lua L;

    EXPECT_TRUE(L.pushFmt("%x", 2).matches(error::PushFmtErrorCode::INVALID_SPECIFIER)) << "pushFmt() did not detect invalid specifier";
    EXPECT_TRUE(L.pushFmt("%").matches(error::PushFmtErrorCode::INVALID_SPECIFIER)) << "pushFmt() did not detect invalid specifier";
    EXPECT_TRUE(L.pushFmt("%", 2).matches(error::PushFmtErrorCode::INVALID_SPECIFIER)) << "pushFmt() did not detect invalid specifier";
    EXPECT_EQ(L.stackSize(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsIncompatibleArgumentsForSpecifiers) {
    Lua L;
    EXPECT_TRUE(L.pushFmt("%d", "123").matches(error::PushFmtErrorCode::INCOMPATIBLE_ARG)) << "pushFmt() did not detect incompatible arguments";
    EXPECT_TRUE(L.pushFmt("%s", 123).matches(error::PushFmtErrorCode::INCOMPATIBLE_ARG)) << "pushFmt() did not detect incompatible argument (%s <- lua_Integer)";
    EXPECT_TRUE(L.pushFmt("%d", LuaNumber{5.5}).matches(error::PushFmtErrorCode::INCOMPATIBLE_ARG)) << "pushFmt() did not detect incompatible argument (%d <- lua_Number)";
    EXPECT_TRUE(L.pushFmt("%f", LuaInteger{5}).matches(error::PushFmtErrorCode::INCOMPATIBLE_ARG)) << "pushFmt() did not detect incompatible argument (%f <- lua_Integer)";

    EXPECT_EQ(L.stackSize(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsExtraSpecifiersAfterAPercentLiteral) {
    Lua L;
    EXPECT_TRUE(L.pushFmt("%% %d").matches(error::PushFmtErrorCode::TOO_FEW_ARGS));
}
