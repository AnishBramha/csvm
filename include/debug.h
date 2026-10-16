#pragma once

#include "chunk.h"


void disassemble_chunk(Chunk*, const char* name);
int disassemble_instruction(Chunk*, int offset);



