#include "./symbols.h"
#include "./tokenizer.h"
#include "./interpreter.h"
#include "./input.h"
#include "interface.h"

int main(void){
	char* line = Calloc(1, MAX_STR_LEN);
	line_t* lines = Malloc(MAX_BASIC_LINES * sizeof(line_t));
	strLines_t* strLines = Malloc(sizeof(strLines_t));
	strLines->head = NULL;
	strLines->tail = NULL;
	strLine_t* curLine = NULL;
	uint32_t lineCnt = 0;

	printString("READY.\n");
	while(1) {
		getLine(line);

		if(stringCmp(line, "LIST\n\0", 5) == 0) {
			printString("\n");
			strLine_t* curL = strLines->head;
			while(curL != NULL) {
				printString(curL->line);
				curL = curL->next;
			}
			printString("READY.\n");
		}else if(stringCmp(line, "RUN\n\0", 4) == 0) {
			lineCnt = lineTokenize(lines, strLines);
			interpret(lines, lineCnt);

			tok_t* curTok  = NULL;
			tok_t* nextTok = NULL;
			for(uint32_t i = 0; i < lineCnt; ++i) {
				curTok = lines[i].firstTok;
				while(curTok != NULL) {
					nextTok = curTok->nextTok;
					free(curTok);
					curTok = nextTok;
				}
				lines[i].firstTok = NULL;
				lines[i].lastTok  = NULL;
				lines[i].num      = 0;
				lines[i].tokCnt   = 0;
			}
		}else if(stringCmp(line, "LISTD\n\0", 6) == 0){
			lineCnt = lineTokenize(lines, strLines);

			tok_t* curTok = NULL;
			for(uint32_t i = 0; i < lineCnt; ++i) {
				curTok = lines[i].firstTok;
				while(curTok != NULL) {
					printTok(curTok);
					curTok = curTok->nextTok;
				}
			}
		}else {
			curLine = mkStrLine(line);
			addStrLine(strLines, curLine);
		}

		line = Calloc(1, MAX_STR_LEN);
	}

	return 0;
}
