#include <gtest/gtest.h>

namespace stronk {

TEST(SyntaxErrorTests, BTest) {
    int result = 2 - 2;
    ASSERT_EQ(result, 0);
}

} // namespace "stronk"
