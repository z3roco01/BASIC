#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "./types.h"
#include "./tokenizer.h"
#include "./interface.h"

u8 interpret(line_t* lines, uint32_t lineCnt);

#endif // INTERPRETER_H_
