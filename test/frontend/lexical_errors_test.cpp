#include <gtest/gtest.h>

TEST(LexicalErrorTests, ATest) {
    int result = 2 - 2;
    ASSERT_EQ(result, 0);
}