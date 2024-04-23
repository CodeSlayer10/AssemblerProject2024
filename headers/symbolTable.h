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
int change_to_entry(Symbol **head, char *name);
void offset_data(Symbol **head, int offset);
void resetSymbolTable(Symbol **head);
