#include <gtest/gtest.h>

TEST(SyntaxErrorTests, BTest) {
    int result = 2 - 2;
    ASSERT_EQ(result, 0);
}