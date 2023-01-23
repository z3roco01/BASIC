#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "./types.h"

uint8_t interpret(line_t* lines, uint32_t lineCnt);

#endif // INTERPRETER_H_
