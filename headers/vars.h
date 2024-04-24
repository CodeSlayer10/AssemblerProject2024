#include "symbolTable.h"
#include "extTable.h"
#include "hashTable.h"

extern unsigned int data[];         // Declaration for an array of unsigned integers holding data
extern unsigned int instructions[]; // Declaration for an array of unsigned integers holding instructions
extern const char base4[4];         // Declaration for a constant character array of size 4 for base 4 representation
int ic;                             // Instruction Counter
int dc;                             // Data Counter
int err;                            // Error flag
int warn;                           // Warning flag
int has_entry;                      // Flag indicating if an entry point exists
int has_external;                   // Flag indicating if there are any external symbols
int has_error;                      // Flag indicating if there were any errors during processing
extern Symbol *symbols;             // Declaration for a pointer to the symbol table
extern external *externals;         // Declaration for a pointer to the external symbols table
extern hashTable *macroTable;       // Declaration for a pointer to the macro table
