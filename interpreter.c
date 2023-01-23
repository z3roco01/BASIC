#include "interpreter.h"
#include "types.h"

uint8_t basicPrint(var_t* vars, tok_t* arg) {
    if(arg->type == STR) {
        printf("%s\n", (char*)arg->data);
    }else if(arg->type == NUM) {
        printf("%i\n", *(int32_t*)arg->data);
    }else if(arg->type == VAR) {
        uint8_t varInd = *(uint8_t*)arg->data;
        switch(vars[varInd].type) {
            case VAR_NUM:
                printf("%i\n", *(int32_t*)vars[varInd].data);
                break;
            case VAR_STR:
                printf("%s\n", (char*)vars[varInd].data);
                break;
            case VAR_VOID:
                break;
            default:
                return 1;
                break;
        }
    }else {
        return 1;
    }
    return 0;
}

uint8_t basicGoto(var_t* vars, tok_t* arg, line_t* lines, uint32_t lineCnt, uint32_t* lineNum) {
    if(arg->type == NUM) {
        uint32_t newLineNum = *(uint32_t*)arg->data;

        for(uint32_t i = 0; i < lineCnt; ++i) {
            if(lines[i].num == newLineNum) {
                *lineNum = i;
                return 0;
            }
        }
    }else if(arg->type == 4) {
        // Non working
        uint8_t varNum = *(uint8_t*)arg->data;

        if(vars[varNum].type != NUM)
            return 1;

        uint32_t newLineNum = *(int32_t*)vars[varNum].data;
        for(uint32_t i = 0; i < lineCnt; ++i) {
            if(lines[i].num == newLineNum) {
                *lineNum = i;
                return 0;
            }
        }
    }
    return 1;
}

uint8_t basicNext(var_t* vars, tok_t* arg) {
    if(arg->type != VAR)
        return 1;

    uint8_t varNum = *(char*)arg->data;

    if(vars[varNum].type != VAR_NUM)
        return 1;

    (*(int32_t*)vars[varNum].data)++;

    return 0;
}

uint8_t execCond(loopCond_t* cond, var_t* vars) {
    switch(cond->chck) {
        case EQ:
            if(vars[cond->varNum].type == NUM && (*(int32_t*)vars[cond->varNum].data) == cond->num+1) {
                return 0;
            }
            break;
        default:
            break;
    }
    return 1;
}

uint8_t interpret(line_t* lines, uint32_t lineCnt) {
    var_t* vars = malloc(sizeof(var_t)*26);
    // Initialize all the varriables
    for(uint8_t i = 0; i < 26; ++i) {
        vars[i].type = VAR_VOID;
        vars[i].data = 0;
    }

    tok_t*      curTok;
    tok_t*      nextTok;
    uint32_t    loopStart = 0;
    uint32_t    loopEnd   = 0;
    uint32_t    looping   = 0;
    loopCond_t* cond      = malloc(sizeof(loopCond_t));
    for(uint32_t i = 0; i < lineCnt; ++i) {
        curTok = lines[i].firstTok;
        //for(uint32_t j = 0; j < lines[i].tokCnt; ++j) {
        while(curTok != NULL) {
            nextTok = curTok->nextTok;
            switch(curTok->type) {
                case NUM:
                    break;
                case STR:
                    break;
                case SYM:
                    switch(*(symbols_t*)curTok->data) {
                        case PRINT:
                            if(basicPrint(vars, curTok->nextTok)) {
                                printf("INVALID ARGUMENT FOR %s ON LINE %u\n", SYMBOLS[PRINT].name, lines[i].num);
                                return 1;
                            }
                            //j++;
                            break;
                        case GOTO:
                            uint32_t newLineInd = 0;
                            if(basicGoto(vars, curTok->nextTok, lines, lineCnt, &newLineInd)) {
                                printf("INALID ARGUMENT FOR %s ON LINE %u\n", SYMBOLS[GOTO].name, lines[i].num);
                                return 1;
                            }
                            i = newLineInd - 1;
                            nextTok = NULL;
                            break;
                        case NEXT:
                            if(basicNext(vars, curTok->nextTok)) {
                                printf("INALID ARGUMENT FOR %s ON LINE %u\n", SYMBOLS[NEXT].name, lines[i].num);
                                return 1;
                            }
                            break;
                        case FOR:
                            loopStart = i+1;
                            uint8_t found = 0;
                            for(uint32_t j = i+1; j < lineCnt; ++j) {
                                if(lines[j].firstTok->nextTok->type == SYM && (*(uint32_t*)lines[j].firstTok->nextTok->data) == NEXT) {
                                    loopEnd = j;
                                    found   = 1;
                                    break;
                                }
                            }

                            if(!found) {
                                printf("NO NEXT TO END THE FOR ON LINE %u\n", lines[i].num);
                                return 1;
                            }

                            if(curTok->nextTok->type != VAR) {
                                printf("VARRIABLE ASSIGNMENT NEEDED AFTER FOR ON LINE %u\n", lines[i].num);
                                return 1;
                            }
                            cond->varNum = *(uint8_t*)curTok->nextTok->data;

                            looping = 1;
                            break;
                        case TO:
                            cond->chck = EQ;
                            if(curTok->nextTok->type != NUM) {
                                printf("NUMBER NEEDED AFTER TO ON LINE %u\n", lines[i].num);
                                return 1;
                            }
                            cond->num  = *(int32_t*)curTok->nextTok->data;
                            break;
                        default:
                            printf("UNKONW SYMBOL WITH NUMBER: %u ON LINE %u\n", *(uint32_t*)curTok->data, lines[i].num);
                            return 1;
                            break;
                    }
                    break;
                case END:
                    nextTok = NULL;
                    if(looping && i == loopEnd && execCond(cond, vars)) {
                        i = loopStart - 1;
                    }
                    break;
                case VAR:
                    break;
                case OP:
                    tok_t* prevTok = curTok->prevTok;
                    tok_t* nextTok = curTok->nextTok;

                    switch(*(uint8_t*)curTok->data) {
                        case ASG:
                            if(prevTok->type != VAR) {
                                printf("CANNOT ASSIGN NON VARRIABLE ON LINE %u", i);
                                return 0;
                            }

                            uint8_t varsInd = *(uint8_t*)prevTok->data;
                            if(nextTok->type == NUM) {
                                vars[varsInd].type = VAR_NUM;
                                vars[varsInd].data = nextTok->data;
                            }
                            break;
                        default:
                            printf("UNKOWN OPERATION WITH NUMBER: %u ON LINE \n", curTok->type, lines[i].num);
                            break;
                    }
                    break;
                default:
                    printf("UNKNONW TOKEN WITH TYPE %u ON LINE\n", curTok->type, lines[i].num);
                    return 1;
                    break;
            }
            curTok = nextTok;
        }
    }

    return 0;
}
