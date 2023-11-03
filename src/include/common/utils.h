#include <vector>
#include <string>
#include "frontend/token.h"
#include "compiler/compiler.h"
#include "common/instruction.h"

/* Utility functions used in source and primarily for testing. */

auto ReadTokensFromSource(const std::string &source) -> std::vector<std::shared_ptr<Token>>;
auto ReadBytecodeFromTokens(const std::vector<std::shared_ptr<Token>> &tokens) -> Bytecode;
auto BuildToken(TokenType token_type) -> std::shared_ptr<Token>;
template <class T> auto BuildValueToken(TokenType token_type, const T &value) -> std::shared_ptr<ValueToken<T>>;
auto BuildTypeToken(TokenType token_type, PrimitiveType type) -> std::shared_ptr<TypeToken>;

auto BuildInstr(OpCode op) -> std::shared_ptr<Instr>;
template <typename... Args> auto BuildInstr(int dest, OpCode op, Args... args) -> std::shared_ptr<PureInstr>;
template <typename... Args> auto BuildInstr(const Address &dest, OpCode op, Args... args) -> std::shared_ptr<PureInstr>;
auto BuildConstInstr(int dest, int index) -> std::shared_ptr<ConstInstr>;
auto BuildConstInstr(Address &dest, int index) -> std::shared_ptr<ConstInstr>;