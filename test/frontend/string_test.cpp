#include <gtest/gtest.h>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"

TEST(StringTests, DISABLED_BasicString) {
    auto token_result = ReadTokensFromSource("strings/single_string.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "Hello, world!"),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0)
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);

    // Case: String with expression in it.
    token_result = ReadTokensFromSource("strings/string_with_expression.stronk");
    token_expected = {
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "5 + 3"),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    bytecode_result = ReadBytecodeFromTokens(token_expected);
    bytecode_expected = {
        BuildConstInstr(1, 0)
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StringTests, DISABLED_FormattedString) {
    auto token_result = ReadTokensFromSource("strings/string_interpolation.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "Number is, "),
        BuildToken(TokenType::DOLLAR_BRACE),
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::STAR),
        BuildValueToken<int>(TokenType::INT, 2),
        BuildToken(TokenType::RIGHT_BRACE),
        BuildValueToken<std::string>(TokenType::TEXT, "!"),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildConstInstr(3, 2),
        BuildInstr(4, OpCode::MULT, 2, 3),
        BuildInstr(5, OpCode::TO_STRING, 4),
        BuildInstr(6, OpCode::CONCAT, 1, 5),
        BuildConstInstr(7, 3),
        BuildInstr(8, OpCode::CONCAT, 6, 7)
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StringTests, DISABLED_NestedString) {
    auto token_result = ReadTokensFromSource("strings/nested_interpolation.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "Nested "),
        BuildToken(TokenType::DOLLAR_BRACE),
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "interpolation?! Are you "),
        BuildToken(TokenType::DOLLAR_BRACE),
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "mad?!"),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::RIGHT_BRACE),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::RIGHT_BRACE),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::TOKEN_EOF),
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildConstInstr(3, 2),
        BuildInstr(4, OpCode::TO_STRING, 3),
        BuildInstr(5, OpCode::CONCAT, 2, 4),
        BuildInstr(6, OpCode::TO_STRING, 5),
        BuildInstr(7, OpCode::CONCAT, 1, 6)
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StringTests, DISABLED_StringInterning) {
    auto token_result = ReadTokensFromSource("strings/string_interning.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "There is "),
        BuildToken(TokenType::DOLLAR_BRACE),
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, " a "),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::RIGHT_BRACE),
        BuildValueToken<std::string>(TokenType::TEXT, " a "),
        BuildToken(TokenType::DOLLAR_BRACE),
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, " way "),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::RIGHT_BRACE),
        BuildValueToken<std::string>(TokenType::TEXT, " way "),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::TOKEN_EOF),
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildInstr(3, OpCode::TO_STRING, 2),
        BuildInstr(4, OpCode::CONCAT, 1, 3),
        BuildConstInstr(5, 1),
        BuildInstr(6, OpCode::CONCAT, 4, 5),
        BuildConstInstr(7, 2),
        BuildInstr(8, OpCode::TO_STRING, 7),
        BuildInstr(9, OpCode::CONCAT, 6, 8),
        BuildConstInstr(10, 2),
        BuildInstr(11, OpCode::CONCAT, 9, 10),
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StringTests, DISABLED_EscapedCharacters) {
    auto token_result = ReadTokensFromSource("strings/escaped_characters.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildToken(TokenType::QUOTE),
        BuildValueToken<std::string>(TokenType::TEXT, "\"Hello\\, world\"\n"),
        BuildToken(TokenType::QUOTE),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0)
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}