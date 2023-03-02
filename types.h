#ifndef TYPES_H_
#define TYPES_H_

#ifdef __linux__
#include <stdint.h>
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint8_t  u8;
#elif __bubblesos__

#endif

#define MAX_BASIC_TOKS        65536
#define MAX_BASIC_TOKS_P_LINE 100
#define MAX_BASIC_LINES       65536
#define MAX_STR_LEN           255
#define MAX_NUM_DIGITS        10
#define SYMS_LEN              7
#define MAX_SYM_LEN           6

typedef struct sym {
    char* name;
    u32 argCount;
} sym_t;

static const sym_t SYMBOLS[SYMS_LEN] = {{"PRINT\0", 1}, {"GOTO\0", 1}, {"NEXT\0", 1}, {"FOR\0", 2}, {"TO\0", 1}, {"IF\0", 4}, {"THEN\0", 1}};

typedef enum symbols {
    PRINT,
    GOTO,
    NEXT,
    FOR,
    TO,
    IF,
    THEN,
} symbols_t;

typedef enum tokType {
    NUM,
    STR,
    SYM,
    END,
    VAR,
    OP,
    COND,
} tokType_t;

typedef struct tok {
    tokType_t   type;
    void*       data;
    struct tok* nextTok;
    struct tok* prevTok;
} tok_t;

typedef struct line {
    u32 num;
    tok_t* firstTok;
    tok_t* lastTok;
    u32 tokCnt;
} line_t;

typedef enum opType {
    ASG,
    ADD,
    SUB,
    MUL,
    DIV,
    NOT,
    AND,
    OR,
    XOR,
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

typedef enum chck {
    EQ,
    LT,
    GT,
    LTE,
    GTE,
    ALWAYS,
} chck_t;

#define CONFIG_VAR 1
#define CONFIG_NUM 0

typedef enum condConfig {
    C_NUM_NUM,
    C_NUM_VAR,
    C_VAR_NUM,
    C_VAR_VAR,
} condConfig_t;

typedef struct cond {
    chck_t   chck;
    u8  config;
    u32 num1;
    u32 num2;
    u8  varNum1;
    u8  varNum2;
} cond_t;

typedef struct strLine {
    struct strLine* prev;
    struct strLine* next;
    char* line;
    u32 num;
} strLine_t;

typedef struct strLines {
    strLine_t* head;
    strLine_t* tail;
} strLines_t;


#endif // TYPES_H_
