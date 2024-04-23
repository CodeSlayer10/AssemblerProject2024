#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "globals.h"

/**
 * Adds a new symbol entry to the symbol table.
 * @param head Pointer to the pointer to the head of the symbol table.
 * @param name The name of the symbol to add.
 * @param value The value associated with the symbol.
 * @param attr The attribute of the symbol.
 */
void addSymbol(Symbol **head, char *name, int value, attribute attr)
{
    // Allocate memory for a new symbol entry.
    Symbol *newEntry = (Symbol *)checkedAlloc(sizeof(Symbol));
    if (newEntry)
    {
        // Allocate memory for the name and copy it.
        newEntry->name = (char *)checkedAlloc(strlen(name) + 1);
        if (newEntry->name)
        {
            strcpy(newEntry->name, name); // Copy the name to the new symbol entry.
            newEntry->value = value; // Set the value of the new symbol entry.
            newEntry->attribute = attr; // Set the attribute of the new symbol entry.

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
 * Finds a symbol with the given name in the symbol table.
 * @param head Pointer to the pointer to the head of the symbol table.
 * @param name The name of the symbol to find.
 * @return A pointer to the symbol with the given name if found, otherwise returns NULL.
 */
Symbol *findSymbol(Symbol **head, char *name)
{
    // Start traversing the symbol table from the head.
    Symbol *current = *head;

    // Iterate through the linked list of symbols.
    while (current != NULL)
    {
        // Compare the name of the current symbol with the target name.
        if (strcmp(current->name, name) == 0)
        {
            return current; // Symbol found, return pointer to the symbol.
        }
        // Move to the next symbol in the symbol table.
        current = current->next;
    }
    // If symbol with the given name is not found, return NULL.
    return NULL;
}


/**
 * Offsets the value of symbols of type DATA in the symbol table by the specified offset.
 * @param head Pointer to the pointer to the head of the symbol table.
 * @param offset The offset value to add to symbols of type DATA.
 */
void offset_data(Symbol **head, int offset)
{
    // Start traversing the symbol table from the head.
    Symbol *current = *head;

    // Iterate through the linked list of symbols.
    while (current != NULL)
    {
        // Check if the current symbol is of type DATA.
        if (current->attribute == DATA)
        {
            // Increment the value of the DATA symbol by the offset.
            current->value += offset;
        }
        // Move to the next symbol in the symbol table.
        current = current->next;
    }
}


/**
 * Locates a symbol with the given name and attribute in the symbol table.
 * @param head Pointer to the pointer to the head of the symbol table.
 * @param name The name of the symbol to locate.
 * @param attr The attribute to match.
 * @return TRUE if a symbol with the given name and attribute is found, FALSE otherwise.
 */
int locateSymbol_by_attribute(Symbol **head, char *name, attribute attr)
{
    // Start traversing the symbol table from the head.
    Symbol *current = *head;

    // Iterate through the symbol table until the end is reached.
    while (current != NULL)
    {
        // If the name and attribute of the current symbol match the given name and attribute, return TRUE.
        if (strcmp(current->name, name) == 0 && current->attribute == attr)
        {
            return TRUE;
        }
        // Move to the next symbol in the symbol table.
        current = current->next;
    }

    // If a symbol with the given name and attribute is not found, return FALSE.
    return FALSE;
}


/**
 * Locates a symbol with the given name in the symbol table.
 * @param head Pointer to the pointer to the head of the symbol table.
 * @param name The name of the symbol to locate.
 * @return TRUE if the symbol with the given name is found, FALSE otherwise.
 */
int locateSymbol(Symbol **head, char *name)
{
    // Start traversing the symbol table from the head.
    Symbol *current = *head;

    // Iterate through the symbol table until the end is reached.
    while (current != NULL)
    {
        // If the name of the current symbol matches the given name, return TRUE.
        if (strcmp(current->name, name) == 0)
        {
            return TRUE;
        }
        // Move to the next symbol in the symbol table.
        current = current->next;
    }

    // If the symbol with the given name is not found, return FALSE.
    return FALSE;
}


/**
 * Changes the attribute of a symbol with the given name to ENTRY.
 * @param head Pointer to the pointer to the head of the symbol table.
 * @param name The name of the symbol to be changed.
 * @return TRUE if the symbol's attribute was successfully changed to ENTRY, FALSE otherwise.
 */
int change_to_entry(Symbol **head, char *name)
{
    // Find the symbol with the given name in the symbol table.
    Symbol *symbol = findSymbol(head, name);

    // If symbol exists, change its attribute to ENTRY and return TRUE.
    if (symbol != NULL)
    {
        symbol->attribute = ENTRY;
        return TRUE;
    }

    // If symbol does not exist, return FALSE.
    return FALSE;
}

/**
 * Resets the symbol table by freeing memory occupied by all entries.
 * @param head Pointer to the pointer to the head of the symbol table.
 */
void resetSymbolTable(Symbol **head)
{
    // Start traversing the symbol table from the head.
    Symbol *current = *head;

    // Iterate through the symbol table.
    while (current != NULL)
    {
        Symbol *temp = current; // Store the current entry in a temporary variable.
        current = current->next; // Move to the next entry in the symbol table.
        free(temp); // Free the memory occupied by the current entry.
    }

    *head = NULL; // Set the head of the symbol table to NULL to indicate an empty table.
}


/*
attribute locateSymbol_by_attribute(Symbol **head, char *name, int num_attributes, ...)
{
    Symbol *current = *head;

    va_list args;
    va_start(args, num_attributes);

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            int found = 1;
            for (int i = 0; i < num_attributes; i++)
            {
                attribute attr = va_arg(args, attribute);
                if (current->attribute != attr)
                {
                    found = 0;
                    break;
                }
            }
            if (found)
            {
                va_end(args);
                return current->attribute; // Attribute found
            }
        }
        current = current->next;
    }

    va_end(args);
    return NONE; // Symbol not found or attributes don't match
}
*/
