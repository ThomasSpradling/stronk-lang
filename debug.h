#ifndef _STRONK_DEBUG_H
#define _STRONK_DEBUG_H

#include <vector>
#include <string_view>
#include "chunk.h"

void disassemble_chunk(const Chunk &chunk, std::string_view name);

#endif // _STRONK_DEBUG_H