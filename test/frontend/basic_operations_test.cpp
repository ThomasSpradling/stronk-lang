#include <gtest/gtest.h>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"

namespace stronk {

TEST(BasicOperationsTests, SimpleInstruction) {
    auto token_result = ReadTokensFromSource("basic_operations/one_instruction.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildValueToken<int>(TokenType::INT, 12),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = { BuildConstInstr(1, 0) };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(BasicOperationsTests, SimpleOperations) {
    auto token_result = ReadTokensFromSource("basic_operations/basic_operations.stronk");
    // 10 * 7.0 / 5 - 2 + 3
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildValueToken<int>(TokenType::INT, 10),
        BuildToken(TokenType::STAR),
        BuildValueToken<float>(TokenType::REAL, 7.0),
        BuildToken(TokenType::SLASH),
        BuildValueToken<int>(TokenType::INT, 5),
        BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 2),
        BuildToken(TokenType::PLUS),
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildInstr(3, OpCode::I2F, 1),
        BuildInstr(4, OpCode::FMULT, 3, 2),
        BuildConstInstr(5, 2),
        BuildInstr(6, OpCode::I2F, 5),
        BuildInstr(7, OpCode::FDIV, 4, 6),
        BuildConstInstr(8, 3),
        BuildInstr(9, OpCode::I2F, 8),
        BuildInstr(10, OpCode::FSUB, 7, 9),
        BuildConstInstr(11, 4),
        BuildInstr(12, OpCode::I2F, 11),
        BuildInstr(13, OpCode::FADD, 10, 12)
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(BasicOperationsTests, Associativity) {
    // Case: 10 - 5 + 3 - 20
    auto token_result = ReadTokensFromSource("basic_operations/basic_associativity.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildValueToken<int>(TokenType::INT, 10),
        BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 5),
        BuildToken(TokenType::PLUS),
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 20),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF),
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildInstr(3, OpCode::SUB, 1, 2),
        BuildConstInstr(4, 2),
        BuildInstr(5, OpCode::ADD, 3, 4),
        BuildConstInstr(6, 3),
        BuildInstr(7, OpCode::SUB, 5, 6),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);

    // Weird case: ---5---6
    token_result = ReadTokensFromSource("basic_operations/unary_associativity.stronk");
    token_expected = {
        BuildToken(TokenType::MINUS), BuildToken(TokenType::MINUS), BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 5),
        BuildToken(TokenType::MINUS), BuildToken(TokenType::MINUS), BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 6),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    bytecode_result = ReadBytecodeFromTokens(token_expected);
    bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildInstr(3, OpCode::SUB, 2, 1),
        BuildConstInstr(4, 1),
        BuildInstr(5, OpCode::SUB, 4, 3),
        BuildConstInstr(6, 1),
        BuildInstr(7, OpCode::SUB, 6, 5),
        BuildConstInstr(8, 2),
        BuildConstInstr(9, 1),
        BuildInstr(10, OpCode::SUB, 9, 8),
        BuildConstInstr(11, 1),
        BuildInstr(12, OpCode::SUB, 11, 10),
        BuildInstr(13, OpCode::SUB, 7, 12),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(BasicOperationsTests, Precedence) {
    // Case: 10 - 2 * 3 + -4
    auto token_result = ReadTokensFromSource("basic_operations/precedence.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildValueToken<int>(TokenType::INT, 10),
        BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 2),
        BuildToken(TokenType::STAR),
        BuildValueToken<int>(TokenType::INT, 3),
        BuildToken(TokenType::PLUS),
        BuildToken(TokenType::MINUS),
        BuildValueToken<int>(TokenType::INT, 4),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF),
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildConstInstr(3, 2),
        BuildInstr(4, OpCode::MULT, 2, 3),
        BuildInstr(5, OpCode::SUB, 1, 4),
        BuildConstInstr(6, 3),
        BuildConstInstr(7, 4),
        BuildInstr(8, OpCode::SUB, 7, 6),
        BuildInstr(9, OpCode::ADD, 5, 8),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

} // namespace "stronk"
