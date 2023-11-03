#include <gtest/gtest.h>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"

TEST(CommentsTest, SingleLineComment) {
    // Single line comment
    auto token_result = ReadTokensFromSource("comments/single_comment.stronk");
    std::vector<std::shared_ptr<Token>> token_expected { BuildToken(TokenType::TOKEN_EOF) };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    ASSERT_EQ(bytecode_result, Bytecode{});

    // Unicode support
    token_result = ReadTokensFromSource("comments/unicode.stronk");
    token_expected = { BuildToken(TokenType::TOKEN_EOF) };

    ASSERT_EQ(token_result, token_expected);

    bytecode_result = ReadBytecodeFromTokens(token_expected);
    ASSERT_EQ(bytecode_result, Bytecode{});
}

TEST(CommentsTest, MultilineComments) {
    // Multiline comment
    auto token_result = ReadTokensFromSource("comments/single_multiline_comment.stronk");
    std::vector<std::shared_ptr<Token>> token_expected { BuildToken(TokenType::TOKEN_EOF) };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    ASSERT_EQ(bytecode_result, Bytecode{});

    // Ensure nesting is fine
    token_result = ReadTokensFromSource("comments/nested_multiline_comment.stronk");
    token_expected = { BuildToken(TokenType::TOKEN_EOF) };

    ASSERT_EQ(token_result, token_expected);

    bytecode_result = ReadBytecodeFromTokens(token_expected);
    ASSERT_EQ(bytecode_result, Bytecode{});
}