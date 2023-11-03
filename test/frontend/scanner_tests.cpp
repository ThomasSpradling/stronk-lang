#include <gtest/gtest.h>

TEST(SimpleMathTests, AdditionTest) {
    int result = 2 + 2;
    ASSERT_EQ(result, 4);
}