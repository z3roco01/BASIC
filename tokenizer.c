#include "tokenizer.h"
#include "types.h"

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
        case COND:
            printf("COND\n");
            break;
        default:
            printf("UNKNOWN TOKEN TYPE: %i\n", tok->type);
            break;
    }
}

tok_t* mkTok(tokType_t type, void* data) {
    tok_t* tok = malloc(sizeof(tok_t));
    tok->type    = type;
    tok->data    = data;
    tok->nextTok = NULL;

    return tok;
}

void addTok(line_t* lines, uint32_t lineInd, tok_t* tok) {
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

uint8_t isChrNum(char c) {
    return (c >= '0' && c <= '9');
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
            if(curStr[i] == '"') {
                // String
                char* str = calloc(1, MAX_STR_LEN+1);
                i++;
                uint32_t j = i;
                while(curStr[j] != '"' && j-i < MAX_STR_LEN) {
                    str[j-i] = curStr[j];
                    j++;
                }

                tok_t* tok = mkTok(STR, str);
                addTok(lines, lineInd, tok);

                i = j;
            }else if(isChrNum(curStr[i])) {
                // Number
                char* numS = calloc(1, MAX_NUM_DIGITS+1);
                uint32_t j = i;
                while(isChrNum(curStr[j]) && j-i < MAX_NUM_DIGITS) {
                    numS[j-i] = curStr[j];
                    j++;
                }

                int32_t* num = calloc(1, sizeof(int32_t));
                *num = atoi(numS);
                free(numS);

                tok_t* tok = mkTok(NUM, num);

                addTok(lines, lineInd, tok);

                i = j;
                i--;
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
                tok_t* tok = NULL;
                tokType_t type = 0;
                void*     data = NULL;
                if(match == 0) {
                    data = malloc(sizeof(uint8_t));
                    *(uint8_t*)data = 0;
                    if(curStr[i] >= 'A' && curStr[i] <= 'Z') {
                        // Var
                        type            = VAR;
                        *(uint8_t*)data = curStr[i] - 0x41;
                    }else if(curStr[i] == '=') {
                        // Asignment
                        type            = OP;
                        *(uint8_t*)data = ASG;
                    }else if(curStr[i] == '+') {
                        type            = OP;
                        *(uint8_t*)data = ADD;
                    }else if(curStr[i] == '-') {
                        type            = OP;
                        *(uint8_t*)data = SUB;
                    }else if(curStr[i] == '*') {
                        type            = OP;
                        *(uint8_t*)data = MUL;
                    }else if(curStr[i] == '/') {
                        type            = OP;
                        *(uint8_t*)data = DIV;
                    }else if(curStr[i] == '!') {
                        type            = OP;
                        *(uint8_t*)data = NOT;
                    }else if(curStr[i] == '&') {
                        type            = OP;
                        *(uint8_t*)data = AND;
                    }else if(curStr[i] == '|') {
                        type            = OP;
                        *(uint8_t*)data = OR;
                    }else if(curStr[i] == '^') {
                        type            = OP;
                        *(uint8_t*)data = XOR;
                    }else if(curStr[i] == '=' && curStr[i+1] == '=') {
                        // matches on ==
                        type            = COND;
                        *(uint8_t*)data = EQ;
                        ++i;
                    }else if(curStr[i] == '>' && curStr[i+1] == '=') {
                        // matches on >=
                        type            = COND;
                        *(uint8_t*)data = GTE;
                        ++i;
                    }else if(curStr[i] == '<' && curStr[i+1] == '=') {
                        // matches on <=
                        type            = COND;
                        *(uint8_t*)data = LTE;
                        ++i;
                    }else if(curStr[i] == '>') {
                        type            = COND;
                        *(uint8_t*)data = GT;
                    }else if(curStr[i] == '<') {
                        type            = COND;
                        *(uint8_t*)data = LT;
                    }
                    free(k);
                }else {
                    type = SYM;
                    data = k;
                    i = j;
                }

                free(symbol);

                tok = mkTok(type, data);

                addTok(lines, lineInd, tok);
            }
        }
        // Add END token
        tok_t* tok = mkTok(END, NULL);

        addTok(lines, lineInd, tok);

        lineInd++;
        lineCnt++;
        curLine = curLine->next;
    }
    return lineCnt;
}
