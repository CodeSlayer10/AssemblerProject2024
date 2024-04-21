#include "globals.h"
#include <stdio.h>

typedef struct Symbol
{
    char *name;
    int value;
    attribute attribute;
    struct Symbol *next;
} Symbol;



void addSymbol(Symbol **head, char *name, int value, attribute attr);
Symbol *findSymbol(Symbol **head, char *name);
int locateSymbol_by_attribute(Symbol **head, char *name, attribute attr);
int locateSymbol(Symbol **head, char *name);
int is_external(Symbol **head, char *name);
int is_define(Symbol **head, char *name);
int change_to_entry(Symbol **head, char *name);
void offset_data(Symbol **head, int offset);
void resetSymbolTable(Symbol **head);

/*
step 1 -> create offset method for first pass - done
step 2 -> create vars to know if there were any entries or external symbols - done
step 3 -> create a linked list for external symbols and implement it.
step 4 -> create the first and second pass methods
step 5 -> create the assembler method in assembler file
step 6 -> fix the pre-assembler
step 7 -> add small things and optimize
step 8 -> ask gpt to create comments and add the error messages handler
*/