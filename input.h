#ifndef INPUT_H_
#define INPUT_H_

#include "./interface.h"
#include "./types.h"

strLine_t* mkStrLine(char* line);
void addStrLine(strLines_t* lines, strLine_t* new);

#endif // INPUT_H_
