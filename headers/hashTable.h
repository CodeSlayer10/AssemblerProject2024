
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

unsigned int hash(char *); 
node *lookup(hashEntry **hashTable, char *key);
void insert(hashEntry **hashTable, char *key, char *line);
hashEntry **initTable();
void resetTable(hashEntry **hashTable);
// implement lookup, insert and reset

#endif // HASHTABLE_H
