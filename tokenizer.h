#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "./types.h"

void printTok(tok_t* tok);
uint32_t lineTokenize(line_t* lines, strLines_t* strLines);

#endif // TOKENIZER_H_
