#ifndef _cmdtable_H
#define _cmdtable_H
#include <stdio.h>

typedef struct code_word
{
    unsigned int ARE : 2;
    unsigned int DEST_ADDRESSING : 2;
    unsigned int SRC_ADDRESSING : 2;
    unsigned int opcode : 4;
    unsigned int padding : 4;
} code_word;

typedef struct data_word
{
    unsigned char ARE : 2;
    unsigned long data : 12;
} data_word;

typedef struct register_word
{
    unsigned int ARE : 2;
    unsigned int SRC_REG : 3;
    unsigned int DEST_REG : 3;
    unsigned int padding : 6;
} reg_word;

typedef union word
{
    code_word code;
    data_word data;
    reg_word reg;

} word;

typedef struct cmdArray
{
    int count;
    int capacity;
    word *commands;
} cmdArray;

cmdArray *commands;

int expandCmdArray(long size);
void addCmd(unsigned int ARE, unsigned int DEST_ADDR, unsigned int SRC_ADDR, unsigned int operation);
void addARG(word arg);
void resetCmdArray();

// cmdEntry *createCmdEntry(word row);
// int insertCMD(unsigned int ARE, unsigned int DEST_ADDR, unsigned int SRC_ADDR, unsigned int opcode);
// int insertARG(word ARG);
// void freeCmdTable();

/*
just going to create an array of commands
and use realloc to change size each time.
*/

// 

#endif