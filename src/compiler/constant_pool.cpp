#include <stdexcept>
#include "compiler/constant_pool.h"

namespace stronk {

// Gets a constant from constant pool by id.
auto ConstantPool::GetConstant(int id) -> ConstantValue {
    if (id < 0 || id >= next_id_) {
        throw std::out_of_range("Invalid constant pool id.");
    }
    return constants_[id];
}

// Adds a constant to constant pool and returns id.
auto ConstantPool::AddConstant(ConstantValue val) -> int {
    if (auto it = value_to_id_.find(val); it != value_to_id_.end()) {
        return it->second;
    }

    int id = next_id_;
    constants_.emplace_back(val);
    value_to_id_[val] = id;
    next_id_++;
    return id;
}

auto ConstantPool::Size() const -> size_t {
    return next_id_;
}

} // namespace "stronk"
