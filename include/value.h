#ifndef _STRONK_VALUE_H
#define _STRONK_VALUE_H

#include <vector>
#include "common.h"

struct Value {
    virtual ~Value() {}
};
struct BoolValue : public Value {
    bool val;
    BoolValue(bool val) : val(val) {} 
};
struct RealValue : public Value {
    float val;
    RealValue(float val) : val(val) {} 
};
struct IntValue : public Value {
    int val;
    IntValue(int val) : val(val) {} 
};
struct CharValue : public Value {
    char val;
    CharValue(char val) : val(val) {} 
};
struct NilValue : public Value {};

using ValueArray = std::vector<Value>;

void PrintValue(Value val);

#endif // _STRONK_VALUE_H