#include "interface.h"

void printString(char* str) {
#ifdef __linux__
    printf("%s", str);
#elif __bubblesos__

#endif
}

void printChar(char c) {
#ifdef __linux__
    printf("%c", c);
#elif __bubblesos__

#endif
}

void printInt(s32 num) {
#ifdef __linux__
    printf("%i", num);
#elif __bubblesos__

#endif
}

void printUint(u32 num) {
#ifdef __linux__
    printf("%ui", num);
#elif __bubblesos__

#endif
}

void printByte(u8 byte) {
#ifdef __linux__
    printf("%hu", byte);
#elif __bubblesos__

#endif
}

void getLine(char* buf) {
#ifdef __linux__
    fgets(buf, MAX_STR_LEN, stdin);
#elif  __bubblesos__

#endif
}

void* Malloc(u32 size) {
#ifdef __linux__
    return malloc(size);
#elif __bubblesos__

#endif
    return NULL;
}

void* Calloc(u32 nmeb, u32 size) {
#ifdef __linux__
    return calloc(nmeb, size);
#elif __bubblesos__

#endif
    return NULL;
}

s32 stringCmp(const char* s1, const char* s2, u32 len) {
#ifdef __linux__
    return strncmp(s1, s2, len);
#elif __bubblesos__

#endif
    return 1;
}
