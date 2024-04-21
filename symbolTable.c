#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "globals.h"
// check if value in symbol table
// add Symbol to symbol table
// find symbol in symbol table and return value

void addSymbol(Symbol **head, char *name, int value, attribute attr)
{
    Symbol *newEntry = (Symbol *)checkedAlloc(sizeof(Symbol));
    if (newEntry)
    {
        newEntry->name = (char *)checkedAlloc(strlen(name) + 1);
        if (newEntry->name)
        {
            strcpy(newEntry->name, name);
            newEntry->value = value;
            newEntry->attribute = attr;
            newEntry->next = *head;
            *head = newEntry;
        }
        else
        {
            free(newEntry);
        }
    }
}

Symbol *findSymbol(Symbol **head, char *name)
{
    Symbol *current = *head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current; // Value found
        }
        current = current->next;
    }
    return NULL; // Symbol not found
}

void offset_data(Symbol **head, int offset)
{
    Symbol *current = *head;
    while (current != NULL)
    {
        if (current->attribute == DATA)
        {
            current->value += offset;
        }
        current = current->next;
    }
}

int locateSymbol_by_attribute(Symbol **head, char *name, attribute attr)
{
    Symbol *current = *head;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0 && current->attribute == attr)
        {
            return TRUE; // Value found
        }
        current = current->next;
    }
    return FALSE; // Symbol not found
}



int locateSymbol(Symbol **head, char *name)
{
    Symbol *current = *head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return TRUE; // Value found
        }
        current = current->next;
    }
    return FALSE; // Symbol not found
}

// int isValueInSymbolTable(struct SymbolTable *table, int value)
// {
//     SymbolEntry *current = table->head;
//     while (current != NULL)
//     {
//         if (current->value == value)
//         {
//             return 1; // Value found
//         }
//         current = current->next;
//     }
//     return FALSE; // Value not found
// }

int change_to_entry(Symbol **head, char *name)
{
    Symbol *symbol = findSymbol(head, name);
    if (symbol != NULL)
    {
        symbol->attribute = ENTRY;
        return TRUE;
    }
    return FALSE;
}


void resetSymbolTable(Symbol **head)
{
    Symbol *current = *head;
    while (current != NULL)
    {
        Symbol *temp = current;
        current = current->next;
        free(temp); // Free the memory occupied by the current entry
    }
    *head = NULL; // Set the head of the table to NULL
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