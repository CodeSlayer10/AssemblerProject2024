
#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASHSIZE 101

typedef struct _node
{
    char *line;
    struct _node *next;
} node;

typedef struct hashEntry
{
    struct hashEntry *next;
    char *key;
    node *lines;
} hashEntry;

typedef struct hashTable
{
    hashEntry *table[HASHSIZE];
} hashTable;

unsigned int hash(char *); 
node *lookup(hashTable *table, char *key);
void insert(hashTable *table, char *key, char *line);
hashTable *initTable();
void resetTable(hashTable *table);
// implement lookup, insert and reset

#endif // HASHTABLE_H
