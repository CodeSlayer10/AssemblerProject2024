#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "vars.h"

/**
 * Computes the hash value for the given key.
 * @param key The input key.
 * @return The computed hash value.
 */
unsigned int hash(char *key)
{
    unsigned int hashval = 0;
    // Calculate the hash value for the given key
    while (*key != '\0')
    {
        hashval = *key + 31 * hashval;
        key++;
    }
    return hashval % HASHSIZE; // Return the hash value within the range of the hash table size
}

/**
 * Looks up a key in the hash table and returns the corresponding node.
 * @param table The hash table.
 * @param key The key to search for.
 * @return The node associated with the key, or NULL if the key is not found.
 */
node *lookup(hashTable *table, char *key)
{
    // Calculate the hash value for the key
    unsigned int hash_index = hash(key);
    // Iterate through the linked list of entries at the calculated hash index
    for (hashEntry *entry = table->table[hash_index]; entry != NULL; entry = entry->next)
    {
        if (strcmp(entry->key, key) == 0)
            return entry->lines; // Return the first node of the linked list of lines
    }
    return NULL; // Key not found
}

/**
 * Inserts a new entry into the hash table.
 * @param table The hash table.
 * @param key The key to insert.
 * @param line The line associated with the key.
 */
void insert(hashTable *table, char *key, char *line)
{
    unsigned int hashval = hash(key);
    hashEntry *entry = table->table[hashval];
    // Iterate through the linked list of entries at the calculated hash index
    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            // Key already exists, add line to existing node
            node *new_node = (node *)malloc(sizeof(node));
            if (new_node == NULL)
            {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            new_node->line = strdup(line);
            if (new_node->line == NULL)
            {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            new_node->next = entry->lines;
            entry->lines = new_node;
            return;
        }
        entry = entry->next;
    }
    // Key doesn't exist, create new entry
    entry = (hashEntry *)malloc(sizeof(hashEntry));
    if (entry == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    entry->key = strdup(key);
    if (entry->key == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    entry->lines = (node *)malloc(sizeof(node));
    if (entry->lines == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    entry->lines->line = strdup(line);
    if (entry->lines->line == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    entry->lines->next = NULL;
    entry->next = table->table[hashval];
    table->table[hashval] = entry;
}

/**
 * Initializes a new hash table.
 * @return A pointer to the newly initialized hash table.
 */
hashTable *initTable()
{
    hashTable *table = (hashTable *)malloc(sizeof(hashTable));
    if (table == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    // Initialize all entries of the hash table to NULL
    for (int i = 0; i < HASHSIZE; i++)
    {
        table->table[i] = NULL;
    }
    return table;
}

/**
 * Resets the hash table, freeing all allocated memory.
 * @param table The hash table to reset.
 */
void resetTable(hashTable *table)
{
    hashEntry *entry;
    node *current;
    // Iterate through all entries of the hash table
    for (int i = 0; i < HASHSIZE; i++)
    {
        entry = table->table[i];
        // Iterate through the linked list of entries at each hash index
        while (entry != NULL)
        {
            hashEntry *tmp = entry->next;
            // Free the key
            free(entry->key);
            // Free the linked list of nodes containing lines
            current = entry->lines;
            while (current != NULL)
            {
                node *tmpNode = current->next;
                free(current->line);
                free(current);
                current = tmpNode;
            }
            free(entry);
            entry = tmp;
        }
        // Set the table entry to NULL after freeing the linked list
        table->table[i] = NULL;
    }
}
