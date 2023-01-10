#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_BASIC_TOKS        65536
#define MAX_BASIC_TOKS_P_LINE 100
#define MAX_BASIC_LINES       65536
#define MAX_STR_LEN           255
#define MAX_NUM_DIGITS        10
#define SYMS_LEN              2
#define MAX_SYM_LEN           6

typedef struct sym {
	char* name;
	uint32_t argCount;
} sym_t;

static const sym_t SYMBOLS[SYMS_LEN] = {{"PRINT\0", 1}, {"GOTO\0", 1}};

typedef enum symbols {
    PRINT,
    GOTO,
} symbols_t;

typedef enum tokType {
    NUM,
	STR,
    SYM,
	END,
	VAR,
	ASG,
} tokType_t;

typedef struct tok {
	tokType_t type;
	void*     data;
	struct tok* nextTok;
} tok_t;

typedef struct line {
	uint32_t num;
	tok_t* firstTok;
	tok_t* lastTok;
	uint32_t tokCnt;
} line_t;

typedef enum varType {
    VAR_NUM,
	VAR_STR,
	VAR_VOID,
} varType_t;

typedef struct var {
	varType_t type;
	void* data;
} var_t;

void printTok(tok_t tok) {
	switch(tok.type) {
		case NUM:
			printf("NUM: %i\n", *(int32_t*)tok.data);
			break;
		case STR:
			printf("STR: %s\n", (char*)tok.data);
			break;
		case SYM:
			printf("SYM: %s\n", SYMBOLS[*(symbols_t*)tok.data].name);
			break;
		case END:
			printf("END\n");
			break;
		default:
			printf("UNKNOWN TOKEN TYPE: %i\n", tok.type);
			break;
	}
}

uint32_t tokenize(line_t* lines, char* code, uint32_t codeLen) {
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
					lines[lineInd].firstTok = tok;
					lines[lineInd].lastTok  = tok;
				}else {
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
					lines[lineInd].firstTok = tok;
					lines[lineInd].lastTok  = tok;
				}else {
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
					lines[lineInd].firstTok = tok;
					lines[lineInd].lastTok  = tok;
				}else {
					lines[lineInd].lastTok->nextTok = tok;
					lines[lineInd].lastTok = tok;
				}
				lines[lineInd].tokCnt++;

				i = j;
			}else {
				// Symbol or var
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
						tok->type    = VAR;
						char letter  = code[i];
						tok->data    = &letter;
						tok->nextTok = NULL;
						free(k);
					}
				}else {
					tok->type = SYM;
					tok->data = k;
					tok->nextTok = NULL;



					i = j;
				}
				free(symbol);

				if(lines[lineInd].firstTok == NULL) {
					lines[lineInd].firstTok = tok;
					lines[lineInd].lastTok  = tok;
				}else {
					lines[lineInd].lastTok->nextTok = tok;
					lines[lineInd].lastTok = tok;
				}

				lines[lineInd].tokCnt++;
			}
		}
	}

	return lineCnt;
}

uint8_t basicPrint(tok_t arg) {
	if(arg.type == STR)
		printf("%s\n", (char*)arg.data);
	else if(arg.type == NUM)
		printf("%i\n", *(int32_t*)arg.data);
	else
		return 1;
	return 0;
}

uint8_t interpret(line_t* lines, uint32_t lineCnt) {
	var_t* vars = malloc(sizeof(var_t)*26);

	tok_t* curTok;
	for(uint32_t i = 0; i < lineCnt; ++i) {
		curTok = lines[i].firstTok;
		//for(uint32_t j = 0; j < lines[i].tokCnt; ++j) {
		while(curTok != NULL) {
			switch(curTok->type) {
				case NUM:
					break;
				case STR:
					break;
				case SYM:
					switch(*(symbols_t*)curTok->data) {
						case PRINT:
							if(basicPrint(*curTok->nextTok)) {
								printf("INVALID ARGUMENT TYPE OF %i FOR %s\n", curTok->nextTok->type, SYMBOLS[PRINT].name);
								return 1;
							}
							//j++;
							break;
						case GOTO:
							break;
						default:
							printf("UNKONW SYMBOL WITH NUMBER: %u\n", *(int32_t*)curTok->data);
							return 1;
							break;
					}
					break;
				case END:
					curTok->nextTok = NULL;
					break;
				default:
					printf("UNKNONW TOKEN WITH TYPE %u\n", curTok->type);
					return 1;
					break;
			}
			curTok = curTok->nextTok;
		}
	}

	return 0;
}

int main(void){
	char* code = "10 A = 5\n20 PRINT A\n\0";
	//char* code = "10 PRINT \"HELLO, WORLD!\"\n\0";

	line_t* lines = malloc(MAX_BASIC_LINES * sizeof(line_t));

	uint32_t lineCnt = tokenize(lines, code, strlen(code));

	return interpret(lines, lineCnt);
}
