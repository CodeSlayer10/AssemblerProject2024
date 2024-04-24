#include "globals.h"
#include <stdio.h>

typedef struct Symbol
{
    char *name;          // Name of the symbol
    int value;           // Value associated with the symbol
    attribute attribute; // Attribute associated with the symbol
    struct Symbol *next; // Pointer to the next symbol in the linked list
} Symbol;                // Definition of a symbol structure

void addSymbol(Symbol **head, char *name, int value, attribute attr);     // Adds a new symbol entry to the symbol table.
Symbol *findSymbol(Symbol **head, char *name);                            // Finds a symbol with the given name in the symbol table.
int locateSymbol_by_attribute(Symbol **head, char *name, attribute attr); // Locates a symbol with the given name and attribute in the symbol table.
int locateSymbol(Symbol **head, char *name);                              // Locates a symbol with the given name in the symbol table.
int change_to_entry(Symbol **head, char *name);                           // Changes the attribute of a symbol with the given name to ENTRY.
void offset_data(Symbol **head, int offset);                              // Offsets the value of symbols of type DATA in the symbol table by the specified offset.
void resetSymbolTable(Symbol **head);                                     // Resets the symbol table by freeing memory occupied by all entries.
