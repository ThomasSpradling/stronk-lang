#include <gtest/gtest.h>
#include <string>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"
#include "common/common.h"

#define P(num) TEMP_VAR_PREFIX + std::to_string(num)

TEST(GlobalVariableTests, BasicVariables) {
    auto token_result = ReadTokensFromSource("variable_assignment/variable_assignment.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildTypeToken(PrimitiveType::INT),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "a"),
        BuildToken(TokenType::EQUAL),
        BuildValueToken<int>(TokenType::INT, 5),
        BuildToken(TokenType::SEMICOLON),
        BuildTypeToken(PrimitiveType::INT),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "b"),
        BuildToken(TokenType::EQUAL),
        BuildValueToken<int>(TokenType::INT, 6),
        BuildToken(TokenType::SEMICOLON),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "a"),
        BuildToken(TokenType::PLUS),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "b"),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr("a", 0),
        BuildConstInstr("b", 1),
        BuildInstr(P(3), OpCode::ADD, "a", "b"),
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(GlobalVariableTests, OverwritingVariables) {
    auto token_result = ReadTokensFromSource("boolean_operations/boolean_operations.stronk");
    std::vector<std::shared_ptr<Token>> token_expected {
        BuildTypeToken(PrimitiveType::BOOL),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "a"),
        BuildToken(TokenType::EQUAL),
        BuildToken(TokenType::TRUE),
        BuildToken(TokenType::SEMICOLON),
        BuildTypeToken(PrimitiveType::BOOL),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "b"),
        BuildToken(TokenType::EQUAL),
        BuildToken(TokenType::FALSE),
        BuildToken(TokenType::SEMICOLON),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "a"),
        BuildToken(TokenType::EQUAL),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "b"),
        BuildToken(TokenType::SEMICOLON),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "b"),
        BuildToken(TokenType::EQUAL),
        BuildToken(TokenType::FALSE),
        BuildToken(TokenType::SEMICOLON),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "a"),
        BuildToken(TokenType::OR),
        BuildValueToken<std::string>(TokenType::IDENTIFIER, "b"),
        BuildToken(TokenType::SEMICOLON),
        BuildToken(TokenType::TOKEN_EOF)
    };

    ASSERT_EQ(token_result, token_expected);

    auto bytecode_result = ReadBytecodeFromTokens(token_expected);
    Bytecode bytecode_expected = {
        BuildConstInstr("a", 0),
        BuildConstInstr("b", 1),
        BuildInstr("a", OpCode::ID, "b"),
        BuildConstInstr("b", 1),
        BuildInstr(P(1), OpCode::OR, "a", "b"),
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}