#ifndef _STRONK_VALUE_H
#define _STRONK_VALUE_H

#include <vector>
#include "common/common.h"

struct Value {
    virtual ~Value() = default;
};
struct BoolValue : public Value {
    bool val_;
    explicit BoolValue(bool val) : val_(val) {} 
};
struct RealValue : public Value {
    float val_;
    explicit RealValue(float val) : val_(val) {} 
};
struct IntValue : public Value {
    int val_;
    explicit IntValue(int val) : val_(val) {} 
};
struct CharValue : public Value {
    char val_;
    explicit CharValue(char val) : val_(val) {} 
};
struct NilValue : public Value {};

using ValueArray = std::vector<Value>;

void PrintValue(Value val);

#endif // _STRONK_VALUE_H