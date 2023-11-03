#include "frontend/token.h"

auto operator==(const std::shared_ptr<Token> &token1, const std::shared_ptr<Token> &token2) -> bool {
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