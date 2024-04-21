#include "datatable.h"
#include "cmdtable.h"
#include "globals.h"
#include <stdlib.h>
#include "utils.h"
// dataEntry createDataEntry(int value);
// int insert(int value);

dataEntry *createDataEntry(int value)
{
    dataEntry *entry = (dataEntry *)checkedAlloc(sizeof(dataEntry));
    entry->value = value;
    entry->next = NULL;
    return entry;
}

int insert(int value)
{
    if (RESERVED_MEMORY + dataEntries->size + commands->capacity > MAX_MEMORY_SIZE)
    {
        return FALSE;
    }
    if (dataEntries->size == 0)
    {
        dataEntries->head = createDataEntry(value);
    }
    else
    {
        dataEntry *current = dataEntries->head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = createDataEntry(value);
    }
    dataEntries->size++;
    return TRUE;
}

void updateSize(int length)
{
    dataEntries->size += length;
}

void resetDataTable()
{
    dataEntry *current = dataEntries->head;
    dataEntry *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    dataEntries->head = NULL;
    dataEntries->size = 0;
}