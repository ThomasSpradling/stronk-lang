#include <gtest/gtest.h>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"

TEST(ComplexExpressionsTests, DISABLED_Precedence) {
    auto token_result = ReadTokensFromSource("complex_expressions/precedence.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::GREATER_EQUAL),
        BuildValueToken<int>(TokenType::INT, 10),
        BuildToken(TokenType::EQUAL),
        BuildToken(TokenType::FALSE),
        BuildToken(TokenType::OR),
        BuildValueToken<int>(TokenType::INT, 5),
        BuildToken(TokenType::BANG_EQUAL),
        BuildValueToken<int>(TokenType::INT, 4),
        BuildToken(TokenType::AND),
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::STAR),
        BuildValueToken<int>(TokenType::INT, 2),
        BuildToken(TokenType::LESS),
        BuildValueToken<int>(TokenType::INT, 6),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildInstr(3, OpCode::GEQ, 1, 2),
        BuildConstInstr(4, 3),
        BuildInstr(5, OpCode::EQ, 3, 4),
        BuildConstInstr(6, 4),
        BuildConstInstr(7, 5),
        BuildInstr(8, OpCode::NEQ, 6, 7),
        BuildConstInstr(9, 0),
        BuildConstInstr(10, 6),
        BuildInstr(11, OpCode::MULT, 9, 10),
        BuildConstInstr(12, 7),
        BuildInstr(13, OpCode::LT, 11, 12),
        BuildInstr(14, OpCode::AND, 8, 13),
        BuildInstr(15, OpCode::OR, 5, 14)
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(ComplexExpressionsTests, DISABLED_Grouping) {
    auto token_result = ReadTokensFromSource("complex_expressions/grouping1.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildValueToken<int>(TokenType::INT, 5),
        BuildToken(TokenType::STAR),
        BuildToken(TokenType::LEFT_PAREN),
        BuildValueToken<int>(TokenType::INT, 1),
        BuildToken(TokenType::PLUS),
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::RIGHT_PAREN),
        BuildToken(TokenType::SLASH),
        BuildValueToken<int>(TokenType::INT, 2),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildConstInstr(3, 2),
        BuildInstr(4, OpCode::ADD, 2, 3),
        BuildInstr(5, OpCode::MULT, 1, 4),
        BuildConstInstr(6, 3),
        BuildInstr(7, OpCode::DIV, 5, 6),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}