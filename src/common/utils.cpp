#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "common/utils.h"
#include "frontend/scanner.h"
#include "config.h" // Generated file from CMakeLists.txt. Make sure to build that first!

namespace stronk {

auto ReadTokensFromSource(const std::string &source) -> std::vector<std::shared_ptr<Token>> {
    std::string base = BASE_DIR;
    std::string filepath = base + "/test/mock/" + source;
    Scanner scanner;
    std::ifstream istream(filepath);

    if (!istream.is_open()) {
        std::cerr << "File does not exist" << "\n";
        exit(74);
    }
    
    // Load source file into scanner buffer.
    std::stringstream buffer;
    buffer << istream.rdbuf();
    std::string str = buffer.str();
    scanner.LoadSource(str);

    std::vector<std::shared_ptr<Token>> tokens;
    for (;;) {
        std::shared_ptr<Token> token = scanner.ScanNextToken();
        tokens.push_back(token);

        if (token->type_ == TokenType::TOKEN_EOF) {
            break;
        }
    }    
    return tokens;
}
auto ReadBytecodeFromTokens(const std::vector<std::shared_ptr<Token>> &tokens) -> Bytecode {
    Parser parser;
    for (const auto &token : tokens) {
        parser.AddToken(token);

        if (token->type_ == TokenType::TOKEN_EOF) {
            break;
        }
    }
    parser.Parse();
    return parser.GetBytecode();
}

auto BuildToken(TokenType token_type) -> std::shared_ptr<Token> {
    return std::make_shared<Token>(token_type, 0, 0);
}

template <class T>
auto BuildValueToken(TokenType token_type, const T &value) -> std::shared_ptr<ValueToken<T>> {
    return std::make_shared<ValueToken<T>>(token_type, 0, 0, value);
}

auto BuildTypeToken(PrimitiveType type) -> std::shared_ptr<TypeToken> {
    return std::make_shared<TypeToken>(TokenType::PRIMITIVE, 0, 0, type, 0);
}

auto BuildInstr(OpCode op) -> std::shared_ptr<Instr> {
    return std::make_shared<Instr>(op, 0, 0);
}

template <typename... Args>
auto BuildInstr(int dest, OpCode op, Args... args) -> std::shared_ptr<PureInstr> {
    std::vector<int> args_vec = {args...};
    std::vector<Address> res_vec;
    for (auto &arg : args_vec) {
        res_vec.push_back(TEMP_VAR_PREFIX + std::to_string(arg));
    }
    Address add = TEMP_VAR_PREFIX + std::to_string(dest);
    return std::make_shared<PureInstr>(op, add, res_vec, 0, 0);
}

template <typename... Args>
auto BuildInstr(OpCode op, Args... args) -> std::shared_ptr<ImpureInstr> {
    std::vector<Address> res_vec = {args...};
    std::vector<Label> labels;
    return std::make_shared<ImpureInstr>(op, res_vec, labels, 0, 0);
}

auto BuildJmp(Label label) -> std::shared_ptr<ImpureInstr> {
    std::vector<Address> args;
    std::vector<Label> labels { label };
    return std::make_shared<ImpureInstr>(OpCode::JMP, args, labels, 0, 0);
}

auto BuildBr(Address arg, Label label1, Label label2) -> std::shared_ptr<ImpureInstr> {
    std::vector<Address> args {arg};
    std::vector<Label> labels { label1, label2 };
    return std::make_shared<ImpureInstr>(OpCode::BR, args, labels, 0, 0);
}

template <typename... Args>
auto BuildInstr(Address dest, OpCode op, Args... args) -> std::shared_ptr<PureInstr> {
    std::vector<Address> res_vec = {args...};
    return std::make_shared<PureInstr>(op, dest, res_vec, 0, 0);
}

auto BuildConstInstr(int dest, int index) -> std::shared_ptr<ConstInstr> {
    Address address = TEMP_VAR_PREFIX + std::to_string(dest);
    return std::make_shared<ConstInstr>(address, index, 0, 0);
}

auto BuildConstInstr(const Address &dest, int index) -> std::shared_ptr<ConstInstr> { 
    return std::make_shared<ConstInstr>(dest, index, 0, 0);
}

auto BuildLabel(Label label) -> std::shared_ptr<LabelInstr> {
    return std::make_shared<LabelInstr>(label, 0, 0);
}


auto operator==(const std::vector<std::shared_ptr<Token>> &list1, const std::vector<std::shared_ptr<Token>> &list2) -> bool {
    if (list1.size() != list2.size()) {
        return false;
    }

    auto it1 = list1.begin();
    auto it2 = list2.begin();
    while (it1 != list1.end()) {
        if (!(**it1 == **it2)) {
            return false;
        }
        it1++;
        it2++;
    }
    return true;
}

auto operator==(const Bytecode &list1, const Bytecode &list2) -> bool {
    if (list1.size() != list2.size()) {
        return false;
    }

    auto it1 = list1.begin();
    auto it2 = list2.begin();
    while (it1 != list1.end()) {
        if (!(**it1 == **it2)) {
            return false;
        }
        it1++;
        it2++;
    }
    return true;
}

template auto BuildInstr(int, OpCode, int) -> std::shared_ptr<PureInstr>;
template auto BuildInstr(int, OpCode, int, int) -> std::shared_ptr<PureInstr>;
template auto BuildInstr(Address, OpCode, const char *, const char *) -> std::shared_ptr<PureInstr>;
template auto BuildInstr(Address, OpCode, const char *) -> std::shared_ptr<PureInstr>;
template auto BuildInstr(OpCode, const char *) -> std::shared_ptr<ImpureInstr>;
template auto BuildValueToken<float>(TokenType, const float &) -> std::shared_ptr<ValueToken<float>>;
template auto BuildValueToken<int>(TokenType, const int&) -> std::shared_ptr<ValueToken<int>>;
template auto BuildValueToken<std::string>(TokenType, const std::string&) -> std::shared_ptr<ValueToken<std::string>>;

} // namespace "stronk"
