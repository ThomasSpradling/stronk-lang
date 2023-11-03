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

// TODO: Generelize a little bit SFINAE
auto AreValueTokensEqual(const std::shared_ptr<Token> &token1, const std::shared_ptr<Token> &token2) -> bool {
    if (token1->type_ == token2->type_ &&
        token1->position_ == token2->position_ &&
        token1->line_ == token2->line_) {
        if (auto valueToken1 = std::dynamic_pointer_cast<ValueToken<int>>(token1)) {
            auto valueToken2 = std::dynamic_pointer_cast<ValueToken<int>>(token2);
            return valueToken2 && valueToken1->value_ == valueToken2->value_;
        } else if (auto valueToken1 = std::dynamic_pointer_cast<ValueToken<float>>(token1)) {
            auto valueToken2 = std::dynamic_pointer_cast<ValueToken<float>>(token2);
            return valueToken2 && valueToken1->value_ == valueToken2->value_;
        } else if (auto valueToken1 = std::dynamic_pointer_cast<ValueToken<std::string>>(token1)) {
            auto valueToken2 = std::dynamic_pointer_cast<ValueToken<std::string>>(token2);
            return valueToken2 && valueToken1->value_ == valueToken2->value_;
        } else if (auto valueToken1 = std::dynamic_pointer_cast<ValueToken<char>>(token1)) {
            auto valueToken2 = std::dynamic_pointer_cast<ValueToken<char>>(token2);
            return valueToken2 && valueToken1->value_ == valueToken2->value_;
        } else if (auto typeToken1 = std::dynamic_pointer_cast<TypeToken>(token1)) {
            auto typeToken2 = std::dynamic_pointer_cast<TypeToken>(token2);
            return typeToken2 && typeToken1->value_ == typeToken2->value_ && typeToken1->width_ == typeToken2->width_;
        } else {
            return true;
        }
    }
    return false;
}

auto CompareTokenVectors(const std::vector<std::shared_ptr<Token>> &vector1, const std::vector<std::shared_ptr<Token>> &vector2) -> bool {
    if (vector1.size() != vector2.size()) {
        return false;
    }

    for (size_t i = 0; i < vector1.size(); ++i) {
        if (!AreValueTokensEqual(vector1[i], vector2[i])) {
            return false;
        }
    }

    return true;
}