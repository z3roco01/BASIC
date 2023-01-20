#include "input.h"

uint32_t getLineNum(char* line) {
    uint32_t i;
    uint32_t num = 0;

    for(i = 0; line[i] >= '0' && line[i] <= '9' && i <= MAX_NUM_DIGITS; ++i)
        num = num * 10 + (line[i] - '0');

    return num;
}

strLine_t* mkStrLine(char* line) {
    strLine_t* strLine = malloc(sizeof(strLine_t));

    strLine->prev = NULL;
    strLine->next = NULL;
    strLine->line = line;
    strLine->num  = getLineNum(line);

    return strLine;
}

void addStrLine(strLines_t* lines, strLine_t* new) {
    if(new == NULL)
        return;
    if(lines->head == NULL) {
        lines->head = new;
        lines->tail = new;
        return;
    }

    if(lines->head == lines->tail) {
        if(lines->head->num < new->num) {
            lines->head->next = new;
            new->prev         = lines->head;

            lines->tail = new;
        }else if(lines->head->num > new->num) {
            lines->head->prev = new;
            new->next         = lines->head;

            lines->head = new;
        }

        return;
    }

    if(lines->head->num == new->num) {
        lines->head->line = new->line;
        return;
    }else if(lines->tail->num == new->num) {
        lines->tail->line = new->line;
        return;
    }

    if(lines->tail->num < new->num) {
        lines->tail->next = new;
        new->prev         = lines->tail;

        lines->tail = new;
        return;
    }

    // curLine will be bigger ->prev will be <
    strLine_t* curLine = lines->head;
    strLine_t* prev;
    strLine_t* next;
    while(curLine != NULL && curLine->num <= new->num) {
        if(curLine->num == new->num) {
            prev = curLine->prev;
            next = curLine->next;

            prev->next = new;
            new->prev = prev;

            next->prev = new;
            new->next = next;
            return;
        }
        if(curLine->next == NULL)
            break;
        curLine = curLine->next;
    }
    prev = curLine->prev;

    prev->next = new;
    new->prev = prev;

    curLine->prev = new;
    new->next = curLine;
}
