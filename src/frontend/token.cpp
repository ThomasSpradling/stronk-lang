#include "frontend/token.h"

bool operator==(const Token &lhs, const Token &rhs) {
    return lhs.type_ == rhs.type_ &&
           lhs.position_ == rhs.position_ &&
           lhs.line_ == rhs.line_;
}

template<class T>
auto operator==(const ValueToken<T> &lhs, const ValueToken<T> &rhs) -> bool {
    return static_cast<const Token &>(lhs) == static_cast<const Token &>(rhs) &&
           lhs.value_ == rhs.value_;
}

auto operator==(const TypeToken &lhs, const TypeToken &rhs) -> bool {
    return static_cast<const ValueToken<PrimitiveType> &>(lhs) == static_cast<const ValueToken<PrimitiveType> &>(rhs) &&
           lhs.width_ == rhs.width_;
}

template auto operator==(const ValueToken<int>& lhs, const ValueToken<int>& rhs) -> bool;
template auto operator==(const ValueToken<std::string>& lhs, const ValueToken<std::string>& rhs) -> bool;
template auto operator==(const ValueToken<float>& lhs, const ValueToken<float>& rhs) -> bool;
template auto operator==(const ValueToken<char>& lhs, const ValueToken<char>& rhs) -> bool;