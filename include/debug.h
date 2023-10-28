#ifndef _STRONK_DEBUG_H
#define _STRONK_DEBUG_H

#include <vector>
#include <stack>
#include <string_view>
#include "chunk.h"
#include "common.h"

auto disassemble_instruction(const Chunk &chunk, int offset) -> int;
void disassemble_chunk(const Chunk &chunk, std::string_view name);
void print_stack(std::stack<Value> &stack);

#endif // _STRONK_DEBUG_H