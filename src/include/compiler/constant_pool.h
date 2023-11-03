#ifndef _STRONK_CONSTANT_POOL_H
#define _STRONK_CONSTANT_POOL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

class ConstantPool {
public:
    using ConstantValue = std::variant<int, float, char, bool, std::string>;
    auto GetConstant(int id) -> ConstantValue;
    auto AddConstant(ConstantValue val) -> int;
    auto Size() const -> size_t;
private:
    std::unordered_map<ConstantValue, int> value_to_id_;
    std::vector<ConstantValue> constants_;
    int next_id_ = 0;
};

#endif // _STRONK_CONSTANT_POOL_H