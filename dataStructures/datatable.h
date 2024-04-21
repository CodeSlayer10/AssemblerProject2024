#ifndef _datatable_H
#define _datatable_H
#include <stdio.h>

typedef struct dataEntry
{
    int value : 12;
    int unused : 2;
    struct dataEntry *next;
} dataEntry;

typedef struct dataTable
{
    dataEntry *head;
    int size;
} dataTable;

dataTable *dataEntries;

dataEntry *createDataEntry(int value);
int insert(int value);
void updateSize(int length);
void resetDataTable();

#endif