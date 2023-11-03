#include <vector>
#include "frontend/token.h"

/* Utility functions used in source and primarily for testing. */

auto ReadTokensFromSource(const std::string &source) -> std::vector<std::shared_ptr<Token>>;
auto AreValueTokensEqual(const std::shared_ptr<Token> &token1, const std::shared_ptr<Token> &token2) -> bool;
auto CompareTokenVectors(const std::vector<std::shared_ptr<Token>> &vector1, const std::vector<std::shared_ptr<Token>> &vector2) -> bool;
