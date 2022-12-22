#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_STR_LEN    200
#define MAX_NUM_DIGITS 10
#define SYMS_LEN       2
#define MAX_SYM_LEN    6
static const char SYMBOLS[SYMS_LEN][MAX_SYM_LEN] = {"PRINT\0", "GOTO\0"};

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
} tok_t;

void printTok(tok_t tok) {
	switch(tok.type) {
		case NUM:
			printf("NUM: %i\n", *((int32_t*)tok.data));
			break;
		case STR:
			printf("STR: %s\n", (char*)tok.data);
			break;
		case SYM:
			printf("SYM: %s\n", SYMBOLS[*(symbols_t*)tok.data]);
			break;
		case END:
			printf("END\n");
			break;
		default:
			printf("UNKNOWN TOKEN TYPE: %i\n", tok.type);
			break;
	}
}

uint32_t tokenize(char* code, tok_t** toks) {
	uint32_t tokCount = 0;
	*toks = malloc(100 * sizeof(tok_t));
	for(uint32_t i = 0; i < strlen(code); ++i) {
		if(code[i] == ' ' || code[i] == '\t' || code[i] == '\n') {

		}else if(code[i] == '\0') {
			break;
		}else if(code[i] == '"') {
			// Parsing a string

			char* str = calloc(1, MAX_STR_LEN+1);
			i++;
			uint32_t j = i;
			while(code[j] != '"' && j-i < MAX_STR_LEN) {
				str[j-i] = code[j];
				j++;
			}

			(*toks)[tokCount].type   = STR;
			(*toks)[tokCount++].data = str;

			i = j;
		}else if(code[i] <= '9' && code[i] >= '0') {
			// Parsing a number

			char* numS = calloc(1, MAX_NUM_DIGITS+1);
			uint32_t j = i;
			while(code[j] >= '0' && code[j] <= '9' && j-i < MAX_NUM_DIGITS) {
				numS[j-i] = code[j];
				j++;
			}


			int32_t num = atoi(numS);
			void* as = &num;

			(*toks)[tokCount].type   = NUM;
			(*toks)[tokCount++].data = &num;
			printf("%s %i\n", numS, *((int32_t*)(*toks)[tokCount-1].data));

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
				if(strcmp(symbol, SYMBOLS[k]) == 0) {
					match = 1;
					break;
				}
			}
			if(match == 0) {
				printf("UNKNOWN SYMBOL: %s\n", symbol);
				return 0;
			}

			(*toks)[tokCount].type   = SYM;
			symbols_t symNum = k;
			(*toks)[tokCount++].data = &symNum;

			i = j;
		}
	}

	(*toks)[tokCount++].type = END;
	return tokCount;
}

int main(void){
	char* code = "10 PRINT \"HELLO WORLD!\"\0";

	tok_t* toks;
	uint32_t tokCount = tokenize(code, &toks);
	for(uint32_t i = 0; i < tokCount; ++i) {
		printTok(toks[i]);
	}

	return 0;
}

