#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "common/utils.h"
#include "frontend/scanner.h"
#include "config.h" // Generated file from CMakeLists.txt. Make sure to build that first!

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
        tokens.push_back(scanner.ScanNextToken());

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