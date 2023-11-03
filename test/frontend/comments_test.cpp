#include <gtest/gtest.h>
#include <memory>
#include "frontend/token.h"
#include "common/utils.h"

TEST(CommentsTest, SingleLineComments) {
    auto result = ReadTokensFromSource("comments/single_comment.stronk");
    std::vector<std::shared_ptr<Token>> expected { std::make_shared<Token>(TokenType::TOKEN_EOF, 0, 1) };
    
    ASSERT_TRUE(CompareTokenVectors(result, expected));
}