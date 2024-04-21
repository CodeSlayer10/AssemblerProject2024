#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "vars.h"

unsigned int hash(char *str)
{
    unsigned hashval;

    for (hashval = 0; *str != '\0'; str++)
    {
        hashval = *str + 31 * hashval;
    }
    return hashval % HASHSIZE;
}
// Function to look up a key in the hash table
node *lookup(hashEntry **hashTable, char *key)
{
    unsigned int index = hash(key); // Use provided hash function
    hashEntry *entry = hashTable[index];

    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry->lines; // Return the lines associated with the key
        }
        entry = entry->next;
    }
    return NULL; // Key not found
}

// Function to insert a key-value pair into the hash table
void insert(hashEntry **hashTable, char *key, char *line)
{
    unsigned int index = hash(key); // Use provided hash function
    hashEntry *entry = hashTable[index];

    // Check if the key already exists
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            // Key found, add the line to the linked list of lines
            node *newNode = (node *)malloc(sizeof(node));
            newNode->line = strdup(line);
            newNode->next = entry->lines;
            entry->lines = newNode;
            return;
        }
        entry = entry->next;
    }

    // Key not found, create a new entry
    hashEntry *newEntry = (hashEntry *)malloc(sizeof(hashEntry));
    newEntry->key = strdup(key);
    newEntry->lines = (node *)malloc(sizeof(node));
    newEntry->lines->line = strdup(line);
    newEntry->lines->next = NULL;
    newEntry->next = hashTable[index];
    hashTable[index] = newEntry;
}

hashEntry **initTable()
{
    hashEntry **hashTable = (hashEntry **)malloc(sizeof(hashEntry *) * HASHSIZE);
    if (hashTable == NULL)
    {
        // Handle memory allocation failure
        return NULL;
    }

    for (int i = 0; i < HASHSIZE; i++)
    {
        hashTable[i] = NULL;
    }

    return hashTable;
}

// Function to reset the hash table
void resetTable(hashEntry **hashTable)
{
    if (hashTable == NULL)
        return;

    for (int i = 0; i < HASHSIZE; i++)
    {
        hashEntry *entry = hashTable[i];
        while (entry != NULL)
        {
            hashEntry *temp = entry;
            entry = entry->next;

            // Free key
            free(temp->key);

            // Free linked list of lines
            node *line = temp->lines;
            while (line != NULL)
            {
                node *lineTemp = line;
                line = line->next;
                free(lineTemp->line);
                free(lineTemp);
            }

            free(temp);
        }
    }

    free(hashTable);
}
