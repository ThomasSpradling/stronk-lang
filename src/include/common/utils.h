#ifndef _STRONK_UTILS_H
#define _STRONK_UTILS_H

#include <vector>
#include <string>
#include "frontend/token.h"
#include "compiler/compiler.h"
#include "common/instruction.h"
#include "common/common.h"

/* Utility functions used in source and primarily for testing. */

namespace stronk {

auto ReadTokensFromSource(const std::string &source) -> std::vector<std::shared_ptr<Token>>;
auto ReadBytecodeFromTokens(const std::vector<std::shared_ptr<Token>> &tokens) -> Bytecode;
auto BuildToken(TokenType token_type) -> std::shared_ptr<Token>;
template <class T> auto BuildValueToken(TokenType token_type, const T &value) -> std::shared_ptr<ValueToken<T>>;
auto BuildTypeToken(PrimitiveType type) -> std::shared_ptr<TypeToken>;

auto BuildInstr(OpCode op) -> std::shared_ptr<Instr>;
template <typename... Args> auto BuildInstr(int dest, OpCode op, Args... args) -> std::shared_ptr<PureInstr>;
template <typename... Args> auto BuildInstr(Address dest, OpCode op, Args... args) -> std::shared_ptr<PureInstr>;
auto BuildConstInstr(const Address &dest, int index) -> std::shared_ptr<ConstInstr>;
auto BuildConstInstr(int dest, int index) -> std::shared_ptr<ConstInstr>;

template <typename... Args> auto BuildInstr(OpCode op, Args... args) -> std::shared_ptr<ImpureInstr>;
auto BuildJmp(Label label) -> std::shared_ptr<ImpureInstr>;
auto BuildBr(Address arg, Label label1, Label label2) -> std::shared_ptr<ImpureInstr>;

auto BuildLabel(Label label) -> std::shared_ptr<LabelInstr>;

auto operator==(const std::vector<std::shared_ptr<Token>> &list1, const std::vector<std::shared_ptr<Token>> &list2) -> bool;
auto operator==(const Bytecode &list1, const Bytecode &list2) -> bool;

} // namespace "stronk"

#endif // _STRONK_UTILS_H