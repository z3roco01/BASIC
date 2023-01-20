#include "tokenizer.h"

void printTok(tok_t* tok) {
    switch(tok->type) {
        case NUM:
            printf("NUM: %i\n", *(int32_t*)tok->data);
            break;
        case STR:
            printf("STR: %s\n", (char*)tok->data);
            break;
        case SYM:
            printf("SYM: %s\n", SYMBOLS[*(symbols_t*)tok->data].name);
            break;
        case END:
            printf("END\n");
            break;
        case VAR:
            printf("VAR: %c\n", (*(char*)tok->data) + 0x41);
            break;
        case OP:
            printf("OP: ");
            switch(*(uint8_t*)tok->data) {
                case ASG:
                    printf("ASG\n");
                    break;
                case ADD:
                    printf("ADD\n");
                    break;
                case SUB:
                    printf("SUB\n");
                    break;
                case MUL:
                    printf("MUL\n");
                    break;
                case DIV:
                    printf("DIV\n");
                    break;
                default:
                    printf("%hu\n", *(uint8_t*)tok->data);
                    break;
            }
            break;
        default:
            printf("UNKNOWN TOKEN TYPE: %i\n", tok->type);
            break;
    }
}

uint32_t lineTokenize(line_t* lines, strLines_t* strLines) {
    uint32_t   lineInd    = 0;
    uint32_t   curLineNum = 0;
    uint32_t   lineCnt    = 0;
    strLine_t* curLine    = strLines->head;
    char*      curStr     = NULL;
    while(curLine != NULL) {
        curLineNum         = curLine->num;
        lines[lineInd].num = curLineNum;
        curStr             = curLine->line;
        for(uint32_t i = 0; curStr[i] != '\0'; ++i) {
            if(curStr[i] == ' ' || curStr[i] == '\t') {
                // Whitespace
            }else if(curStr[i] == '\n') {
                // End of line

                tok_t* tok = malloc(sizeof(tok_t));
                tok->type    = END;
                tok->data    = NULL;
                tok->nextTok = NULL;

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }

            }else if(curStr[i] == '"') {
                // String
                char* str = calloc(1, MAX_STR_LEN+1);
                i++;
                uint32_t j = i;
                while(curStr[j] != '"' && j-i < MAX_STR_LEN) {
                    str[j-i] = curStr[j];
                    j++;
                }

                tok_t* tok = malloc(sizeof(tok_t));
                tok->type    = STR;
                tok->data    = str;
                tok->nextTok = NULL;

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }
                lines[lineInd].tokCnt++;

                i = j;
            }else if(curStr[i] >= '0' && curStr[i] <= '9') {
                // Number
                char* numS = calloc(1, MAX_NUM_DIGITS+1);
                uint32_t j = i;
                while(curStr[j] >= '0' && curStr[j] <= '9' && j-i < MAX_NUM_DIGITS) {
                    numS[j-i] = curStr[j];
                    j++;
                }

                int32_t* num = calloc(1, sizeof(int32_t));
                *num = atoi(numS);
                free(numS);

                tok_t* tok = malloc(sizeof(tok_t));
                tok->type    = NUM;
                tok->data    = num;
                tok->nextTok = NULL;

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }
                lines[lineInd].tokCnt++;

                i = j;
            }else {
                // Symbol, var or op
                char* symbol = calloc(1, MAX_SYM_LEN);
                uint32_t j = i;
                while(curStr[j] != ' ' && curStr[j] != '(' && curStr[j] != '\0' && j-i < MAX_SYM_LEN-1) {
                    symbol[j-i] = curStr[j];
                    j++;
                }

                uint32_t* k = malloc(sizeof(uint32_t));
                uint8_t match = 0;
                for((*k) = 0; *k < SYMS_LEN; ++(*k)) {
                    if(strcmp(symbol, SYMBOLS[*k].name) == 0) {
                        match = 1;
                        break;
                    }
                }
                tok_t* tok = malloc(sizeof(tok_t));
                if(match == 0) {

                    if(curStr[i] >= 'A' && curStr[i] <= 'Z') {
                        // Var
                        tok->type       = VAR;
                        uint8_t* letter = malloc(sizeof(uint8_t));
                        *letter         = curStr[i] - 0x41;
                        tok->data       = letter;
                        tok->nextTok    = NULL;
                        free(k);
                    }else if(curStr[i] == '=') {
                        // Asignment
                        tok->type    = OP;
                        uint8_t* op  = malloc(sizeof(uint8_t));
                        *op          = ASG;
                        tok->data    = op;
                        tok->nextTok = NULL;
                    }
                }else {
                    tok->type = SYM;
                    tok->data = k;
                    tok->nextTok = NULL;

                    i = j;
                }
                free(symbol);

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }

                lines[lineInd].tokCnt++;
            }
        }
        lineInd++;
        lineCnt++;
        curLine = curLine->next;
    }
    return lineCnt;
}

/*uint32_t tokenize(line_t* lines, char* code, uint32_t codeLen) {
    uint8_t  gotLineNum = 0;
    uint32_t lineInd    = 0;
    uint32_t curLineNum = 0;
    uint32_t lineCnt    = 0;
    for(uint32_t i = 0; i < codeLen; ++i) {
        if(!gotLineNum) {
            char* numS = calloc(MAX_NUM_DIGITS+1, 1);
            uint32_t j;
            for(j = i; j-i < MAX_NUM_DIGITS && code[j] >= '0' && code[j] <= '9'; ++j)
                numS[j-i] = code[j];

            curLineNum = atoi(numS);
            free(numS);

            lines[lineInd].num      = curLineNum;
            lines[lineInd].tokCnt   = 0;
            lines[lineInd].firstTok = NULL;
            lines[lineInd].lastTok  = NULL;
            gotLineNum = 1;

            lineCnt++;
            i = j;
        }else {
            if(code[i] == ' ' || code[i] == '\t') {
                // Whitespace
            }else if(code[i] == '\n' || code[i] == '\0') {
                // End of line

                tok_t* tok = malloc(sizeof(tok_t));
                tok->type    = END;
                tok->data    = NULL;
                tok->nextTok = NULL;

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }

                lineInd++;
                gotLineNum = 0;
            }else if(code[i] == '"') {
                // String
                char* str = calloc(1, MAX_STR_LEN+1);
                i++;
                uint32_t j = i;
                while(code[j] != '"' && j-i < MAX_STR_LEN) {
                    str[j-i] = code[j];
                    j++;
                }

                tok_t* tok = malloc(sizeof(tok_t));
                tok->type    = STR;
                tok->data    = str;
                tok->nextTok = NULL;

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }
                lines[lineInd].tokCnt++;

                i = j;
            }else if(code[i] >= '0' && code[i] <= '9') {
                // Number
                char* numS = calloc(1, MAX_NUM_DIGITS+1);
                uint32_t j = i;
                while(code[j] >= '0' && code[j] <= '9' && j-i < MAX_NUM_DIGITS) {
                    numS[j-i] = code[j];
                    j++;
                }

                int32_t* num = calloc(1, sizeof(int32_t));
                *num = atoi(numS);
                free(numS);

                tok_t* tok = malloc(sizeof(tok_t));
                tok->type    = NUM;
                tok->data    = num;
                tok->nextTok = NULL;

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }
                lines[lineInd].tokCnt++;

                i = j;
            }else {
                // Symbol, var or op
                char* symbol = calloc(1, MAX_SYM_LEN);
                uint32_t j = i;
                while(code[j] != ' ' && code[j] != '(' && code[j] != '\0' && j-i < MAX_SYM_LEN-1) {
                    symbol[j-i] = code[j];
                    j++;
                }

                uint32_t* k = malloc(sizeof(uint32_t));
                uint8_t match = 0;
                for((*k) = 0; *k < SYMS_LEN; ++(*k)) {
                    if(strcmp(symbol, SYMBOLS[*k].name) == 0) {
                        match = 1;
                        break;
                    }
                }
                tok_t* tok = malloc(sizeof(tok_t));
                if(match == 0) {

                    if(code[i] >= 'A' && code[i] <= 'Z') {
                        // Var
                        tok->type       = VAR;
                        uint8_t* letter = malloc(sizeof(uint8_t));
                        *letter         = code[i] - 0x41;
                        tok->data       = letter;
                        tok->nextTok    = NULL;
                        free(k);
                    }else if(code[i] == '=') {
                        // Asignment
                        tok->type    = OP;
                        uint8_t* op  = malloc(sizeof(uint8_t));
                        *op          = ASG;
                        tok->data    = op;
                        tok->nextTok = NULL;
                    }
                }else {
                    tok->type = SYM;
                    tok->data = k;
                    tok->nextTok = NULL;

                    i = j;
                }
                free(symbol);

                if(lines[lineInd].firstTok == NULL) {
                    tok->prevTok = NULL;
                    lines[lineInd].firstTok = tok;
                    lines[lineInd].lastTok  = tok;
                }else {
                    tok->prevTok = lines[lineInd].lastTok;
                    lines[lineInd].lastTok->nextTok = tok;
                    lines[lineInd].lastTok = tok;
                }

                lines[lineInd].tokCnt++;
            }
        }
    }

    return lineCnt;
}*/
