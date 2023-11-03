#include <gtest/gtest.h>
#include <memory>
#include "frontend/token.h"
#include "common/utils.h"
#include "frontend/compiler.h"

TEST(CommentsTest, SingleLineComments) {
    auto token_result = ReadTokensFromSource("comments/single_comment.stronk");
    std::vector<std::shared_ptr<Token>> token_expected { std::make_shared<Token>(TokenType::TOKEN_EOF, 0, 1) };
    
    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    ASSERT_EQ(bytecode_result, Bytecode{});
}