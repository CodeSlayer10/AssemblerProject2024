#ifndef _cmdHandlers_H
#define _cmdHandlers_H
#include "globals.h"

opcode find_operation(char *line, int *index);
addressing_type get_addressing_type(char *operand);
int operationHandler(char *line, addressing_type *first_operand, addressing_type *second_operand);
#endif