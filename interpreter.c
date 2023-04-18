#include "interpreter.h"

// Round and absolute
u32 rabs(float f) {
    // round up or down?
    u8 add = (f - (s32)f < 0.5 ? 0 : 1);

    s32 i = ((s32)f) + add;

    // get absolute value
    if(i >= 0)
        return (u32)i;
    else
        return (u32)i * -1;
}

enum {
    RESULT_SUCCESS = 0,
    RESULT_ERROR_INVALID_TYPE,
};

int basicPrint(var_t* vars, tok_t* arg) {
    if(arg->type == STR) {
        printString((char*)arg->data);
        printString("\n");
    }else if(arg->type == NUM) {
        printInt(*(s32*)arg->data);
        printString("\n");
    }else if(arg->type == VAR) {
        u8 varInd = *(u8*)arg->data;
        switch(vars[varInd].type) {
            case VAR_NUM:
                printInt(*(s32*)vars[varInd].data);
                printString("\n");
                return RESULT_SUCCESS;
            case VAR_STR:
                printString((char*)vars[varInd].data);
                printString("\n");
                return RESULT_SUCCESS;
            case VAR_VOID:
                return RESULT_SUCCESS;
            default:
                return RESULT_ERROR_INVALID_TYPE;
        }
    }else {
        return RESULT_ERROR_INVALID_TYPE;
    }
    return RESULT_SUCCESS;
}

u8 basicGoto(var_t* vars, tok_t* arg, line_t* lines, u32 lineCnt, u32* lineNum) {
    if(arg->type == NUM) {
        u32 newLineNum = *(u32*)arg->data;

        for(u32 i = 0; i < lineCnt; ++i) {
            if(lines[i].num == newLineNum) {
                *lineNum = i;
                return 0;
            }
        }
    }else if(arg->type == 4) {
        // Non working
        u8 varNum = *(u8*)arg->data;

        if(vars[varNum].type != NUM)
            return 1;

        u32 newLineNum = *(s32*)vars[varNum].data;
        for(u32 i = 0; i < lineCnt; ++i) {
            if(lines[i].num == newLineNum) {
                *lineNum = i;
                return 0;
            }
        }
    }
    return 1;
}

u8 basicNext(var_t* vars, tok_t* arg) {
    if(arg->type != VAR)
        return 1;

    u8 varNum = *(char*)arg->data;

    if(vars[varNum].type != VAR_NUM)
        return 1;

    (*(s32*)vars[varNum].data)++;

    return 0;
}

u32 condGetLeft(cond_t* cond, var_t* vars) {
    if(cond->config & 0b10)
        return *(u32*)vars[cond->varNum1].data;
    else
        return cond->num1;
}

u32 condGetRight(cond_t* cond, var_t* vars) {
    if(cond->config & 0b01)
        return *(u32*)vars[cond->varNum2].data;
    else
        return cond->num2;
}

u8 execCond(cond_t* cond, var_t* vars) {
    switch(cond->chck) {
        case EQ:
            if(condGetLeft(cond, vars) == condGetRight(cond, vars))
                return 0;
            break;
        case LT:
            if(condGetLeft(cond, vars) < condGetRight(cond, vars))
                return 0;
            break;
        case GT:
            if(condGetLeft(cond, vars) > condGetRight(cond, vars))
                return 0;
            break;
        case LTE:
            if(condGetLeft(cond, vars) <= condGetRight(cond, vars))
                return 0;
            break;
        case GTE:
            if(condGetLeft(cond, vars) >= condGetRight(cond, vars))
                return 0;
            break;
        default:
            printString("UNKNOWN COND ");
            printUint(cond->chck);
            printString(" !\n");
            break;
    }
    return 1;
}


void printErr(char* err, u32 lineNum) {
    printString(err);
    printString(" ON LINE ");
    printUint(lineNum);
    printChar('\n');
}

void printSymErr(symbols_t sym, u32 lineNum) {
    printString("INVALID ARGUMENT FOR ");
    printString(SYMBOLS[PRINT].name);
    printString(" ON LINE ");
    printUint(lineNum);
    printChar('\n');

}


u8 interpret(line_t* lines, u32 lineCnt) {
    var_t* vars = Malloc(sizeof(var_t)*26);
    // Initialize all the varriables
    for(u8 i = 0; i < 26; ++i) {
        vars[i].type = VAR_VOID;
        vars[i].data = 0;
    }

    tok_t*      curTok;
    tok_t*      nextTok;
    u32    loopStart = 0;
    u32    loopEnd   = 0;
    u32    looping   = 0;
    cond_t* cond      = Malloc(sizeof(cond_t));
    for(u32 i = 0; i < lineCnt; ++i) {
        curTok = lines[i].firstTok;

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
                                printSymErr(PRINT, lines[i].num);
                                return 1;
                            }
                            //j++;
                            break;
                        case GOTO:
                            u32 newLineInd = 0;
                            if(basicGoto(vars, curTok->nextTok, lines, lineCnt, &newLineInd)) {
                                printSymErr(GOTO, lines[i].num);
                                return 1;
                            }
                            i = newLineInd - 1;
                            nextTok = NULL;
                            break;
                        case NEXT:
                            if(basicNext(vars, curTok->nextTok)) {
                                printSymErr(NEXT, lines[i].num);
                                return 1;
                            }
                            break;
                        case FOR:
                            loopStart = i+1;
                            u8 found = 0;
                            for(u32 j = i; j < lineCnt; ++j) {
                                // Temp work around for tokenizing that makes extra num tokens at the start of lines
                                if(lines[j].firstTok->nextTok->nextTok->type == SYM && (*(u32*)lines[j].firstTok->nextTok->nextTok->data) == NEXT) {
                                    loopEnd = j;
                                    found   = 1;
                                    break;
                                }
                            }

                            if(!found) {
                                printErr("NO NEXT TO END THE FOR", lines[i].num);
                                return 1;
                            }

                            if(curTok->nextTok->type != VAR) {
                                printErr("VARRIABLE ASSIGNMENT NEEDED AFTER FOR", lines[i].num);
                                return 1;
                            }
                            cond->config = C_VAR_NUM;
                            cond->varNum1 = *(u8*)curTok->nextTok->data;

                            looping = 1;
                            break;
                        case TO:
                            cond->chck = EQ;
                            if(curTok->nextTok->type != NUM) {
                                printErr("NUMBER NEEDED AFTER TO", lines[i].num);
                                return 1;
                            }
                            cond->num2 = *(u32*)curTok->nextTok->data;
                            break;
                        case IF:
                            tok_t* left = curTok->nextTok;
                            tok_t* chck = left->nextTok;
                            tok_t* right = chck->nextTok;
                            if((left->type != VAR && left->type != NUM) || chck->type != COND || (right->type != VAR && right->type != NUM)) {
                                printErr("WRONG ARGUMENTS FOR IF STATEMENT", lines[i].num);
                                return 1;
                            }

                            cond_t* ifCond = Malloc(sizeof(cond_t));
                            ifCond->config = 0;
                            ifCond->chck = *(u8*)chck->data;
                            // Parse the config (which are variables and arent) and set the right var/num
                            if(left->type == VAR) {
                                // set left bit to var type
                                ifCond->config  = 0b10;
                                ifCond->varNum1 = *(u8*)left->data;
                            }else {
                                // clear left bit to num type
                                ifCond->num1 = *(u32*)left->data;
                            }

                            if(right->type == VAR) {
                                // set right bit to var type
                                ifCond->config |= 0b01;
                                ifCond->varNum2 = *(u8*)right->data;
                            }else {
                                // clear right bit to num type
                                ifCond->num2 = *(u32*)right->data;
                            }
                            if(execCond(ifCond, vars))
                                nextTok = NULL;

                            break;
                        case THEN:
                            break;
                        default:
                            printString("UNKONW SYMBOL WITH NUMBER: ");
                            printUint(*(u32*)curTok->data);
                            printString(" ON LINE ");
                            printUint(lines[i].num);
                            printChar('\n');
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

                    switch(*(u8*)curTok->data) {
                        case ASG:
                            if(prevTok->type != VAR && (nextTok->type != NUM || nextTok->type != VAR)) {
                                printErr("CANNOT ASSIGN NON VARIABLE TO NON VARIABLE/NUMBER", i);
                                return 0;
                            }
                            u8 varsInd = *(u8*)prevTok->data;
                            if(nextTok->nextTok->type == OP) {
                                // operation that needs to be execed
                                tok_t* opTok = nextTok->nextTok;

                                tok_t* leftTok  = opTok->prevTok;
                                tok_t* rightTok = opTok->nextTok;

                                u32 leftNum  = 0;
                                u32 rightNum = 0;

                                if(leftTok->type == NUM) {
                                    leftNum = *(u32*)leftTok->data;
                                }else if(leftTok->type == VAR) {
                                    u8 lvar = *(u8*)leftTok->data;
                                    if(vars[lvar].type != VAR_NUM) {
                                        printErr("VARIABLE IS NOT OF TYPE NUMBER FOR ADD", lines[i].num);
                                    }
                                    leftNum = *(u32*)vars[lvar].data;
                                }

                                if(rightTok->type == NUM) {
                                    rightNum = *(u32*)rightTok->data;
                                }else if(rightTok->type == VAR) {
                                    u8 lvar = *(u8*)rightTok->data;
                                    if(vars[lvar].type != VAR_NUM) {
                                        printErr("VARIABLE IS NOT OF TYPE NUMBER FOR ADD", lines[i].num);
                                    }
                                    rightNum = *(u32*)vars[lvar].data;
                                }
                                u32 res  = 0;
                                vars[varsInd].type = VAR_NUM;
                                switch (*(u8*)opTok->data) {
                                    case ADD:
                                        res = leftNum + rightNum;
                                        vars[varsInd].data = &res;
                                        break;
                                    case SUB:
                                        res = leftNum - rightNum;
                                        vars[varsInd].data = &res;
                                        break;
                                    case MUL:
                                        res = rabs(leftNum * rightNum);
                                        vars[varsInd].data = &res;
                                        break;
                                    case DIV:
                                        res = rabs(leftNum / rightNum);
                                        vars[varsInd].data = &res;
                                        break;
                                    case AND:
                                        res = leftNum & rightNum;
                                        vars[varsInd].data = &res;
                                        break;
                                    case OR:
                                        res = leftNum | rightNum;
                                        vars[varsInd].data = &res;
                                        break;
                                    case XOR:
                                        res = leftNum ^ rightNum;
                                        vars[varsInd].data = &res;
                                        break;
                                    default:
                                        break;
                                }
                            }else {
                                if(nextTok->type == NUM) {
                                    vars[varsInd].type = VAR_NUM;
                                    vars[varsInd].data = nextTok->data;
                                }else if(nextTok->type == VAR) {
                                    u8 nextVarInd = *(u8*)nextTok->data;
                                    vars[varsInd].type = vars[nextVarInd].type;
                                    vars[varsInd].data = vars[nextVarInd].data;
                                }
                            }

                            break;
                        case ADD:
                            break;
                        case SUB:
                            break;
                        case MUL:
                            break;
                        case DIV:
                            break;
                        case NOT:
                            break;
                        case AND:
                            break;
                        case OR:
                            break;
                        case XOR:
                            break;
                        default:
                            printString("UNKOWN OPERATION WITH NUMBER: ");
                            printUint(curTok->type);
                            printString(" ON LINE ");
                            printUint(lines[i].num);
                            printChar('\n');
                            break;
                    }
                    break;
                case COND:
                    break;
                default:
                    printString("UNKOWN TOKEN WITH TYPE: ");
                    printUint(curTok->type);
                    printString(" ON LINE ");
                    printUint(lines[i].num);
                    printChar('\n');
                    return 1;
                    break;
            }
            curTok = nextTok;
        }
    }

    return 0;
}
