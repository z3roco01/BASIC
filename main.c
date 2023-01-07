#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_BASIC_TOKS        65536
#define MAX_BASIC_TOKS_P_LINE 100
#define MAX_BASIC_LINES       65536
#define MAX_STR_LEN           200
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

/*uint32_t tokenize(char* code, tok_t* toks) {
	uint32_t tokCount    = 0;
	uint8_t  gotLineNum  = 0;
	uint32_t curLineNum  = 0;
	uint32_t gettingArgs = 0;

	for(uint32_t i = 0; i < strlen(code); ++i) {
		if(!gotLineNum && !gettingArgs) {
				char* numS = calloc(1, MAX_NUM_DIGITS+1);
				uint32_t j = i;
				while(code[j] >= '0' && code[j] <= '9' && j-i < MAX_NUM_DIGITS && code[j] != ' ') {
					if(code[j] > '9' && code[j] < '0'){
						printf("NOT A VALID LINE NUMBER");
						free(numS);
						return 1;
					}
					numS[j-i] = code[j];
					j++;
				}
				i = j;
				curLineNum = atoi(numS);
				gotLineNum = 1;
		}else {
			if(code[i] == ' ' || code[i] == '\t' || code[i] == '\n' || code[i] == '\0') {

			}else if(code[i] == '"') {
				// Parsing a string

				char* str = calloc(1, MAX_STR_LEN+1);
				i++;
				uint32_t j = i;
				while(code[j] != '"' && j-i < MAX_STR_LEN) {
					str[j-i] = code[j];
					j++;
				}

				toks[tokCount].type   = STR;
				toks[tokCount++].data = str;

				i = j;
			}else if(code[i] <= '9' && code[i] >= '0') {
				// Parsing a number

				char* numS = calloc(1, MAX_NUM_DIGITS+1);
				uint32_t j = i;
				while(code[j] >= '0' && code[j] <= '9' && j-i < MAX_NUM_DIGITS) {
					numS[j-i] = code[j];
					j++;
				}

				int32_t* num = malloc(sizeof(int32_t));
				*num = atoi(numS);

				toks[tokCount].type   = NUM;
				toks[tokCount++].data = num;

				i = j;
			}else {
				// Parsing a symbol

				char* symbol = calloc(1, MAX_SYM_LEN);
				uint32_t j = i;
				while(code[j] != ' ' && code[j] != '(' && code[j] != '\0' && j-i < MAX_SYM_LEN-1) {
					symbol[j-i] = code[j];
					j++;
				}

				uint32_t k;
				uint8_t match = 0;
				for(k = 0; k < SYMS_LEN; ++k) {
					if(strcmp(symbol, SYMBOLS[k].name) == 0) {
						match = 1;
						gettingArgs = SYMBOLS[k].argCount;
						break;
					}
				}
				if(match == 0) {
					printf("UNKNOWN SYMBOL: %s\n", symbol);
					return 0;
				}

				toks[tokCount].type   = SYM;
				symbols_t symNum = k;
				toks[tokCount++].data = &symNum;

				i = j;
			}

			toks[tokCount-1].lineNum = curLineNum;
			gotLineNum = 0;
		}
	}

	toks[tokCount++].type = END;
	return tokCount;
}*/

uint32_t newTokenize(line_t* lines, char* code, uint32_t codeLen) {
	uint8_t  gotLineNum = 0;
	uint32_t lineInd    = 0;
	uint32_t curLineNum = 0;
	for(uint32_t i = 0; i < codeLen; ++i) {
		if(!gotLineNum) {
			char* numS = calloc(MAX_NUM_DIGITS+1, 1);
			uint32_t j;
			for(j = i; j-i < MAX_NUM_DIGITS && code[j] >= '0' && code[j] <= '9'; ++j)
				numS[j-i] = code[j];

			curLineNum = atoi(numS);
			free(numS);

			printf("%i\n", curLineNum);

			lines[lineInd].num      = curLineNum;
			lines[lineInd].tokCnt   = 0;
			lines[lineInd].firstTok = NULL;
			lines[lineInd].lastTok  = NULL;
			gotLineNum = 1;
			i = j;
		}else {
			if(code[i] == ' ' || code[i] == '\t' || code[i] == '\0') {
				// Whitespace
			}else if(code[i] == '\n') {
				// End of line
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

				tok_t* tok = malloc(sizeof(tok));
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
				// Symbol
				char* symbol = calloc(1, MAX_SYM_LEN);
				uint32_t j = i;
				while(code[j] != ' ' && code[j] != '(' && code[j] != '\0' && j-i < MAX_SYM_LEN-1) {
					symbol[j-i] = code[j];
					j++;
				}

				uint32_t k;
				uint8_t match = 0;
				for(k = 0; k < SYMS_LEN; ++k) {
					if(strcmp(symbol, SYMBOLS[k].name) == 0) {
						match = 1;
						break;
					}
				}
				if(match == 0) {
					printf("UNKNOWN SYMBOL: %s\n", symbol);
					return 0;
				}

				tok_t* tok = malloc(sizeof(tok_t));
				tok->type = SYM;
				symbols_t symNum = k;
				tok->data = &symNum;
				tok->nextTok = NULL;

				free(symbol);

				if(lines[lineInd].firstTok == NULL) {
					lines[lineInd].firstTok = tok;
					lines[lineInd].lastTok  = tok;
				}else {
					lines[lineInd].lastTok->nextTok = tok;
					lines[lineInd].lastTok = tok;
				}
				lines[lineInd].tokCnt++;

				i = j;
			}
		}
	}
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

// returns 0 on success anything else is failure
/*uint8_t interpret(tok_t* toks, uint32_t tokCount) {
	for(uint32_t i = 0; i < tokCount; ++i) {
		switch(toks[i].type) {
			case NUM:
				break;
			case STR:
				break;
			case SYM:
				switch(*(int32_t*)toks[i].data) {
					case PRINT:
						if(toks[i+1].lineNum == toks[i].lineNum && basicPrint(toks[i+1])) {
							printf("INVALID ARGUMENT TYPE OF %i FOR %s\n", toks[i].type, SYMBOLS[PRINT]);
						}
						i++;
						break;
					case GOTO:
						break;
					default:
						printf("UNKONW SYMBOL WITH NUMBER: %i", *(int32_t*)toks[i].data);
						break;
				}
				break;
			case END:
				return 0;
				break;
			default:
				printf("UNKOWN TOKEN: %i", toks[i].type);
				return 1;
				break;
		}
	}

	return 0;
}*/

int main(void){
	char* code = "10 PRINT \"HELLO WORLD!\"\n20 PRINT \"BALLS\"\0";

	line_t* lines = malloc(MAX_BASIC_LINES * sizeof(line_t));

	uint32_t lineCnt = newTokenize(lines, code, strlen(code));

	tok_t* nextTok = lines[0].firstTok;
	while(nextTok != NULL) {
		printTok(*nextTok);
		nextTok = nextTok->nextTok;
	}
	/*tok_t* toks;
	toks = malloc(MAX_BASIC_TOKS * sizeof(tok_t));
	uint32_t tokCount = tokenize(code, toks);
	//char* line = calloc(1, MAX_STR_LEN+1);
	while(1){
		fgets(line, MAX_STR_LEN, stdin);
		uint32_t tokCount = tokenize(line, toks);
		uint8_t result = interpret(toks, tokCount);
	}
	for(uint32_t i = 0; i < tokCount; ++i)
		printTok(toks[i]);

	return interpret(toks, tokCount);*/
}

