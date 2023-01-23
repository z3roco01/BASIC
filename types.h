#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

#define MAX_BASIC_TOKS        65536
#define MAX_BASIC_TOKS_P_LINE 100
#define MAX_BASIC_LINES       65536
#define MAX_STR_LEN           255
#define MAX_NUM_DIGITS        10
#define SYMS_LEN              5
#define MAX_SYM_LEN           6

typedef struct sym {
    char* name;
    uint32_t argCount;
} sym_t;

static const sym_t SYMBOLS[SYMS_LEN] = {{"PRINT\0", 1}, {"GOTO\0", 1}, {"NEXT\0", 1}, {"FOR\0", 2}, {"TO\0", 1}};

typedef enum symbols {
    PRINT,
    GOTO,
    NEXT,
    FOR,
    TO,
} symbols_t;

typedef enum tokType {
    NUM,
    STR,
    SYM,
    END,
    VAR,
    OP,
} tokType_t;

typedef struct tok {
    tokType_t   type;
    void*       data;
    struct tok* nextTok;
    struct tok* prevTok;
} tok_t;

typedef struct line {
    uint32_t num;
    tok_t* firstTok;
    tok_t* lastTok;
    uint32_t tokCnt;
} line_t;

typedef enum opType {
    ASG,
    ADD,
    SUB,
    MUL,
    DIV,
} opType_t;

typedef enum varType {
    VAR_NUM,
    VAR_STR,
    VAR_VOID,
} varType_t;

typedef struct var {
    varType_t type;
    void* data;
} var_t;

typedef enum loopChck {
    EQ,
    LT,
    GT,
    LTE,
    GTE,
    ALWAYS,
} loopChck_t;

typedef struct loopCond {
    loopChck_t chck;
    int32_t    num;
    uint8_t    varNum;
} loopCond_t;

typedef struct strLine {
    struct strLine* prev;
    struct strLine* next;
    char* line;
    uint32_t num;
} strLine_t;

typedef struct strLines {
    strLine_t* head;
    strLine_t* tail;
} strLines_t;


#endif // TYPES_H_
