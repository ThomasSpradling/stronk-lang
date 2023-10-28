#ifndef _STRONK_DEBUG_H
#define _STRONK_DEBUG_H

#include <vector>
#include <string_view>
#include "chunk.h"

auto disassemble_instruction(const Chunk &chunk, int offset) -> int;
void disassemble_chunk(const Chunk &chunk, std::string_view name);

#endif // _STRONK_DEBUG_H