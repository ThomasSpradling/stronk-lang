#include <gtest/gtest.h>
#include "frontend/token.h"
#include "common/utils.h"
#include "compiler/compiler.h"

#define P(num) (TEMP_VAR_PREFIX + std::to_string(num)).c_str()

TEST(StatementsTests, DISABLED_PrintStatement) {
    auto token_result = ReadTokensFromSource("statements/print_basic.stronk");

    auto bytecode_result = ReadBytecodeFromTokens(token_result);
    Bytecode bytecode_expected = {
        BuildInstr(OpCode::PRINT, "a"),
        BuildConstInstr(1, 0),
        BuildInstr(OpCode::PRINT, P(1)),
        BuildConstInstr(2, 1),
        BuildInstr(OpCode::PRINT, P(2)),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StatementsTests, DISABLED_BasicIfStatement) {
    auto token_result = ReadTokensFromSource("complex_expressions/if_basic.stronk");

    auto bytecode_result = ReadBytecodeFromTokens(token_result);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildConstInstr(2, 1),
        BuildInstr(3, OpCode::ADD, 1, 2),
        BuildConstInstr(4, 2),
        BuildInstr(5, OpCode::EQ, 3, 4),
        BuildBr(P(5), ".if_0.true", ".if_0.false"),
        BuildLabel(".if_0.true"),
        BuildConstInstr(6, 3),
        BuildInstr(OpCode::PRINT, P(2)),
        BuildJmp(".if_0.exit"),
        BuildLabel(".if_0.false"),
        BuildConstInstr(7, 4),
        BuildInstr(OpCode::PRINT, P(7)),
        BuildLabel(".if_0.exit"),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StatementsTests, DISABLED_AloneIfStatement) {
    auto token_result = ReadTokensFromSource("complex_expressions/alone_if.stronk");

    auto bytecode_result = ReadBytecodeFromTokens(token_result);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildBr(P(1), ".if_0.true", ".if_0.false"),
        BuildLabel(".if_0.true"),
        BuildConstInstr(2, 1),
        BuildInstr(OpCode::PRINT, P(2)),
        BuildLabel(".if_0.false"),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}

TEST(StatementsTests, DISABLED_BasicWhileStatement) {
    auto token_result = ReadTokensFromSource("complex_expressions/while_basic.stronk");

    auto bytecode_result = ReadBytecodeFromTokens(token_result);
    Bytecode bytecode_expected = {
        BuildConstInstr(1, 0),
        BuildInstr("i", OpCode::ID, P(1)),
        BuildLabel(".while_0.cond"),
        BuildConstInstr(2, 1),
        BuildInstr(P(3), OpCode::LT, "i", P(2)),
        BuildBr(P(3), ".while_0.true", ".while_0.exit"),
        BuildLabel(".while_0.true"),
        BuildConstInstr(4, 2),
        BuildInstr(P(5), OpCode::ADD, "i", P(4)),
        BuildInstr("i", OpCode::ID, P(5)),
        BuildJmp(".while_0.true"),
        BuildLabel(".while_0.exit"),
    };
    ASSERT_EQ(bytecode_result, bytecode_expected);
}