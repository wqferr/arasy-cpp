#include <gtest/gtest.h>
#include "arasy.hpp"

using namespace arasy::core;

TEST(PushFmt, DoesntMangleSimpleStrings) {
    using arasy::error::PushFmtError;
    Lua L;
    EXPECT_EQ(L.pushFmt("this is a simple string"), PushFmtError::NONE)
        << "pushFmt() expected arguments even in the absence of placeholders";
    auto str = L.getTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*str, "this is a simple string") << "pushFmt() mangled a simple string with no arguments";
}

TEST(PushFmt, DoesntMangleCorrectlyNotatedArguments) {
    Lua L;
    EXPECT_EQ(L.pushFmt("literal then %s (%d) -> %f%%", "interpolated", 123, 0.5), arasy::error::PushFmtError::NONE)
        << "pushFmt() incorrectly identified errors in simple interpolation";
    auto str = L.getTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(str, "literal then interpolated (123) -> 0.5%") << "pushFmt() mangled a simple formatting job";
}

TEST(PushFmt, UnderstandsAllIntegerFormats) {
    Lua L;
    EXPECT_EQ(L.pushFmt("%d %c", 100, 65), arasy::error::PushFmtError::NONE);
    auto str = L.getTop<LuaString>();
    ASSERT_NE(str, std::nullopt) << "pushFmt() did not push result onto the stack";
    EXPECT_EQ(*str, "100 A");
}

TEST(PushFmt, DetectsTooFewArguments) {
    Lua L;
    EXPECT_EQ(L.pushFmt("%d %d %d", 1, 2), arasy::error::PushFmtError::TOO_FEW_ARGS) << "pushFmt() did not detect lack of arguments for placeholders";
    EXPECT_EQ(L.pushFmt("%f"), arasy::error::PushFmtError::TOO_FEW_ARGS) << "pushFmt() did not detect lack of arguments for placeholders when there are no arguments";
    EXPECT_EQ(L.size(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsTooManyArguments) {
    Lua L;
    EXPECT_EQ(L.pushFmt("%d", 1, 2), arasy::error::PushFmtError::TOO_MANY_ARGS) << "pushFmt() did not detect excess of arguments for placeholders";
    EXPECT_EQ(L.pushFmt("string with no placeholders", "this should not be used"), arasy::error::PushFmtError::TOO_MANY_ARGS) << "pushFmt() did not detect excess of arguments for placeholders when there are no placeholders";
    EXPECT_EQ(L.size(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsInvalidSpecifiers) {
    Lua L;
    EXPECT_EQ(L.pushFmt("%x", 2), arasy::error::PushFmtError::INVALID_SPECIFIER) << "pushFmt() did not detect invalid specifier";
    EXPECT_EQ(L.pushFmt("%"), arasy::error::PushFmtError::INVALID_SPECIFIER) << "pushFmt() did not detect invalid specifier";
    EXPECT_EQ(L.pushFmt("%", 2), arasy::error::PushFmtError::INVALID_SPECIFIER) << "pushFmt() did not detect invalid specifier";
    EXPECT_EQ(L.size(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}

TEST(PushFmt, DetectsIncompatibleArgumentsForSpecifiers) {
    Lua L;
    EXPECT_EQ(L.pushFmt("%d", "123"), arasy::error::PushFmtError::INCOMPATIBLE_ARG) << "pushFmt() did not detect incompatible argument (%d <- str)";
    EXPECT_EQ(L.pushFmt("%s", 123), arasy::error::PushFmtError::INCOMPATIBLE_ARG) << "pushFmt() did not detect incompatible argument (%s <- lua_Integer)";
    EXPECT_EQ(L.pushFmt("%d", LuaNumber{5.5}), arasy::error::PushFmtError::INCOMPATIBLE_ARG) << "pushFmt() did not detect incompatible argument (%d <- lua_Number)";
    EXPECT_EQ(L.pushFmt("%f", LuaInteger{5}), arasy::error::PushFmtError::INCOMPATIBLE_ARG) << "pushFmt() did not detect incompatible argument (%f <- lua_Integer)";
    EXPECT_EQ(L.size(), 0) << "Invalid invocations of pushFmt() still pushed values onto the stack";
}
