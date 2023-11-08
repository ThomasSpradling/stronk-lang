#include <gtest/gtest.h>

namespace stronk {

TEST(LexicalErrorTests, ATest) {
    int result = 2 - 2;
    ASSERT_EQ(result, 0);
}

} // namespace "stronk"
