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
node *lookup(hashTable *table, char *key) {
    hashEntry *entry;
    for (entry = table->table[hash(key)]; entry != NULL; entry = entry->next) {
        if (strcmp(entry->key, key) == 0)
            return entry->lines; // Return the first node of the linked list of lines
    }
    return NULL; // Key not found
}

void insert(hashTable *table, char *key, char *line) {
    unsigned int hashval = hash(key);
    hashEntry *entry = table->table[hashval];
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            // Key already exists, add line to existing node
            node *new_node = (node *)malloc(sizeof(node));
            if (new_node == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            new_node->line = strdup(line);
            new_node->next = entry->lines;
            entry->lines = new_node;
            return;
        }
        entry = entry->next;
    }
    // Key doesn't exist, create new entry
    entry = (hashEntry *)malloc(sizeof(hashEntry));
    if (entry == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    entry->key = strdup(key);
    entry->lines = (node *)malloc(sizeof(node));
    if (entry->lines == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    entry->lines->line = strdup(line);
    entry->lines->next = NULL;
    entry->next = table->table[hashval];
    table->table[hashval] = entry;
}

hashTable *initTable() {
    hashTable *table = (hashTable *)malloc(sizeof(hashTable));
    if (table == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < HASHSIZE; i++) {
        table->table[i] = NULL;
    }
    return table;
}

// void resetTable(hashTable *table) {
//     for (int i = 0; i < HASHSIZE; i++) {
//         hashEntry *entry = table->table[i];
//         while (entry != NULL) {
//             hashEntry *next_entry = entry->next;
//             node *line = entry->lines;
//             while (line != NULL) {
//                 node *next_line = line->next;
//                 free(line->line);
//                 free(line);
//                 line = next_line;
//             }
//             free(entry->key);
//             free(entry);
//             entry = next_entry;
//         }
//         table->table[i] = NULL;
//     }
//     free(table);
// }

void resetTable(hashTable *table)
{
    hashEntry *entry;
    node *current;
    for (int i = 0; i < HASHSIZE; i++)
    {
        entry = table->table[i];
        while (entry != NULL)
        {
            hashEntry *tmp = entry->next;
            free(entry->key);
            current = entry->lines;
            while (current != NULL)
            {
                node *tmpNode = current->next;
                free(current);
                current = tmpNode;
            }
            free(entry);
            entry = tmp;
        }
    }
}