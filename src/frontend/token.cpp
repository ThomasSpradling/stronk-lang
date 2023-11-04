#include "frontend/token.h"

auto operator==(const std::shared_ptr<Token> &token1, const std::shared_ptr<Token> &token2) -> bool {
    if (token1->type_ == token2->type_) {
        if (auto value_token_1 = std::dynamic_pointer_cast<ValueToken<int>>(token1)) {
            auto value_token_2 = std::dynamic_pointer_cast<ValueToken<int>>(token2);
            return value_token_2 && value_token_1->value_ == value_token_2->value_;
        } else if (auto value_token_1 = std::dynamic_pointer_cast<ValueToken<float>>(token1)) {
            auto value_token_2 = std::dynamic_pointer_cast<ValueToken<float>>(token2);
            return value_token_2 && value_token_1->value_ == value_token_2->value_;
        } else if (auto value_token_1 = std::dynamic_pointer_cast<ValueToken<std::string>>(token1)) {
            auto value_token_2 = std::dynamic_pointer_cast<ValueToken<std::string>>(token2);
            return value_token_2 && value_token_1->value_ == value_token_2->value_;
        } else if (auto value_token_1 = std::dynamic_pointer_cast<ValueToken<char>>(token1)) {
            auto value_token_2 = std::dynamic_pointer_cast<ValueToken<char>>(token2);
            return value_token_2 && value_token_1->value_ == value_token_2->value_;
        } else if (auto type_token_1 = std::dynamic_pointer_cast<TypeToken>(token1)) {
            auto type_token_2 = std::dynamic_pointer_cast<TypeToken>(token2);
            return type_token_2 && type_token_1->value_ == type_token_2->value_;
        } else {
            return true;
        }
    }
    return false;
}