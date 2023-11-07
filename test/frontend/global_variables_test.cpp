#include <gtest/gtest.h>
#include <string>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"
#include "common/common.h"

#define P(num) (TEMP_VAR_PREFIX + std::to_string(num)).c_str()

TEST(GlobalVariableTests, BasicVariables) {
    auto token_result = ReadTokensFromSource("global_variables/basic_variables.stronk");
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
        BuildConstInstr(P(1), 0),
        BuildInstr("a", OpCode::ID, P(1)),
        BuildConstInstr(P(2), 1),
        BuildInstr("b", OpCode::ID, P(2)),
        BuildInstr(P(3), OpCode::ADD, "a", "b"),
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(GlobalVariableTests, OverwritingVariables) {
    auto token_result = ReadTokensFromSource("global_variables/overwriting_variables.stronk");
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
        BuildConstInstr(P(1), 0),
        BuildInstr("a", OpCode::ID, P(1)),
        BuildConstInstr(P(2), 1),
        BuildInstr("b", OpCode::ID, P(2)),
        BuildInstr("a", OpCode::ID, "b"),
        BuildConstInstr(P(3), 1),
        BuildInstr("b", OpCode::ID, P(3)),
        BuildInstr(P(4), OpCode::OR, "a", "b"),
    };

    ASSERT_EQ(bytecode_result, bytecode_expected);
}