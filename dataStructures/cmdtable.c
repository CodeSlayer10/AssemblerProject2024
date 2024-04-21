#include "globals.h"
#include "utils.h"
#include "cmdtable.h"
#include "datatable.h"
#include <stdlib.h>

// add a command entry
/*
int expandCmdArray(long size);
int addCmd(unsigned int ARE, unsigned int DEST_ADDR, unsigned int SRC_ADDR, unsigned int opcode);
int addARG(word arg, int position);
void freeCmdArray();
*/

int expandCmdArray(long size)
{
    if (RESERVED_MEMORY + dataEntries->size + commands->capacity > MAX_MEMORY_SIZE)
    {
        return FALSE;
    }

    word *new_commands = checkedRealloc(commands->commands, (commands->capacity + size) * sizeof(word));
    if (new_commands == NULL)
    {
        return FALSE; /* Failed to allocate memory */
    }
    commands->commands = new_commands;
    commands->capacity += size;
    return TRUE; /* Success */
}

void addCmd(unsigned int ARE, unsigned int DEST_ADDR, unsigned int SRC_ADDR, unsigned int operation)
{ // assuming length smaller than capacity
    
    word cw;
    cw.code.ARE = ARE;
    cw.code.DEST_ADDRESSING = DEST_ADDR;
    cw.code.SRC_ADDRESSING = SRC_ADDR;
    cw.code.opcode = operation;
    commands->commands[commands->count] = cw;
    commands->count++;
}

void addARG(word arg)
{
    commands->commands[commands->count] = arg;
    commands->count++;
}

void resetCmdArray()
{
    free(commands->commands);
    commands->commands = NULL;
    commands->count = 0;
}