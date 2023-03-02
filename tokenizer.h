#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include "./types.h"
#include "./interface.h"

void printTok(tok_t* tok);
uint32_t lineTokenize(line_t* lines, strLines_t* strLines);

#endif // TOKENIZER_H_
