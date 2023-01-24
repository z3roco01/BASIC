#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "./symbols.h"
#include "./tokenizer.h"
#include "./interpreter.h"
#include "./input.h"

int main(void){
	char* line = calloc(1, MAX_STR_LEN);
	line_t* lines = malloc(MAX_BASIC_LINES * sizeof(line_t));
	strLines_t* strLines = malloc(sizeof(strLines_t));
	strLines->head = NULL;
	strLines->tail = NULL;
	strLine_t* curLine = NULL;
	uint32_t lineCnt = 0;

	printf("READY.\n");
	while(1) {
		fgets(line, MAX_STR_LEN, stdin);

		if(strncmp(line, "LIST\n\0", 5) == 0) {
			printf("\n");
			strLine_t* curL = strLines->head;
			while(curL != NULL) {
				printf("%s", curL->line);
				curL = curL->next;
			}
			printf("READY.\n");
		}else if(strncmp(line, "RUN\n\0", 4) == 0) {
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
		}else if(strncmp(line, "LISTD\n\0", 6) == 0){
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

		/*curL = lines->head;
		while(curL != NULL) {
			printf("    %s\n", curL->line);
			curL = curL->next;
		}*/

		line = calloc(1, MAX_STR_LEN);
	}

	return 0;
}
