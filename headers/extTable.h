
typedef struct external
{
    char *name;            // Name of the external symbol
    int address;           // Address associated with the external symbol
    struct external *next; // Pointer to the next external symbol in the linked list
} external;                // Definition of an external symbol structure

void add_ext(external **head, char *name, int address); // Adds a new external entry to the external symbol table.

void reset_ext(external **head); // Resets the external symbol table by freeing memory occupied by all entries.
