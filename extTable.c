#include <stdio.h>
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "extTable.h"

/**
 * Adds a new external entry to the external symbol table.
 * @param head Pointer to the pointer to the head of the external symbol table.
 * @param name The name of the external symbol to add.
 * @param address The address associated with the external symbol.
 */
void add_ext(external **head, char *name, int address)
{
    // Allocate memory for a new external entry.
    external *newEntry = (external *)checkedAlloc(sizeof(external));
    if (newEntry)
    {
        // Allocate memory for the name and copy it.
        newEntry->name = (char *)checkedAlloc(strlen(name) + 1);
        if (newEntry->name)
        {
            strcpy(newEntry->name, name); // Copy the name to the new external entry.
            newEntry->address = address; // Set the address of the new external entry.

            // Insert the new entry at the beginning of the list.
            newEntry->next = *head; // Point the next pointer of the new entry to the current head.
            *head = newEntry; // Update the head to point to the new entry.
        }
        else
        {
            // Free the allocated entry if memory allocation fails for the name.
            free(newEntry);
        }
    }
}

/**
 * Resets the external symbol table by freeing memory occupied by all entries.
 * @param head Pointer to the pointer to the head of the external symbol table.
 */
void reset_ext(external **head)
{
    // Start traversing the external symbol table from the head.
    external *current = *head;

    // Iterate through the external symbol table.
    while (current != NULL)
    {
        // Store the next pointer before freeing the current entry.
        external *next = current->next;

        // Free the memory occupied by the name and the entry itself.
        free(current->name);
        free(current);

        // Move to the next entry in the external symbol table.
        current = next;
    }

    // Set the head of the external symbol table to NULL to indicate an empty table.
    *head = NULL;
}
