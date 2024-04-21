#include "symbolTable.h"
#include "extTable.h"
#include "hashTable.h"

extern unsigned int data[];
extern unsigned int instructions[];
extern const char base4[4];
int ic;
int dc;
int err;
int warn;
int has_entry;
int has_external;
int has_error;
extern Symbol *symbols;
extern external *externals;
extern hashEntry **macroTable;
