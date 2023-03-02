#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "types.h"

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#elif __bubblesos__

#endif

// OS specific code goes here

void printString(char* str);
void printChar(char c);
void printInt(s32 num);
void printUint(u32 num);
void printByte(u8 byte);

void getLine(char* buf);

void* Malloc(u32 size);
void* Calloc(u32 nmeb, u32 size);
s32 stringCmp(const char* s1, const char* s2, u32 len);

#endif // INTERFACE_H_
