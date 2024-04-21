#include <stdio.h>
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "extTable.h"

void add_ext(external **head, char *name, int address)
{

    external *newEntry = (external *)checkedAlloc(sizeof(external));
    if (newEntry)
    {
        newEntry->name = (char *)checkedAlloc(strlen(name) + 1);
        if (newEntry->name)
        {
            strcpy(newEntry->name, name);
            newEntry->address = address;
            newEntry->next = *head;
            *head = newEntry;
        }
        else
        {
            free(newEntry);
        }
    }
}

void reset_ext(external **head)
{
    // Start from the head of the list
    external *current = *head;
    while (current != NULL)
    {
        // Store the next pointer before freeing the current entry
        external *next = current->next;

        // Free the name and the entry itself
        free(current->name);
        free(current);

        // Move to the next entry
        current = next;
    }

    // Set the head of the table to NULL, indicating an empty list
    *head = NULL;
}