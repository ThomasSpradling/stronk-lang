#ifndef _STRONK_DEBUG_H
#define _STRONK_DEBUG_H

#include <vector>
#include <stack>
#include <string_view>
#include "chunk.h"
#include "common.h"

auto DisassembleInstruction(const Chunk &chunk, int offset) -> int;
void DisassembleChunk(const Chunk &chunk, std::string_view name);
void PrintStack(std::stack<Value> &stack);

#endif // _STRONK_DEBUG_H