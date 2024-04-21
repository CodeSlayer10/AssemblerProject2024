#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dataHandlers.h"
#include "cmdHandlers.h"
#include "vars.h"
#include "first_pass.h"

void first_pass(FILE *fp)
{
    char line[LINESIZE + 1];
    int line_num = 1;

    ic = 0;
    dc = 0;
    err = FALSE;
    has_error = FALSE;

    while (fgets(line, LINESIZE + 1, fp) != NULL)
    {

        if (!process_line(line))
        {
            has_error = TRUE;
            print_error_message(err, line_num);
            // write_error(line_num);
        }
        line_num++;
    }

    offset_data(&symbols, ic + RESERVED_MEMORY);
}

int process_line(char *line)
{

    instruction instruction;
    char symbol[SYMBOL_MAX_SIZE + 1];
    int index = 0;
    MOVE_TO_NOT_WHITE(line, index);

    find_label(line, symbol);
    if (symbol[0] && !is_valid_symbol(symbol))
    {
        // error: invalid symbol name [name]
        return FALSE;
    }
    if (symbol[0])
    {
        for (; line[index] != ':'; index++)
            ;
        index++;
    }

    MOVE_TO_NOT_WHITE(line, index);

    if (is_end_of_line(line[index]))
    {
        warn = WARNING_LABEL_ONLY;
        return TRUE; // warning empty symbol: [name]
    }
    if (findSymbol(&symbols, symbol))
    {
        err = LABEL_ALREADY_EXISTS;
        // error: symbol [name] already exists
        return FALSE;
    }

    instruction = find_instruction(line, &index);

    if (instruction == DEFINE_IN && symbol[0] != '\0')
    {
        err = DEFINE_CANT_HAVE_LABEL;
        // error: define cannot have a symbol
        return FALSE;
    }

    if ((instruction == STRING_IN || instruction == DATA_IN) && symbol[0] != '\0')
    {
        addSymbol(&symbols, symbol, dc, DATA);
    }
    MOVE_TO_NOT_WHITE(line, index);
    if (instruction != NONE_IN)
    {
        switch (instruction)
        {
        case DEFINE_IN:
            return defineHandler(&line[index]);
        case STRING_IN:
            return stringHandler(&line[index]);
        case DATA_IN:
            return dataHandler(&line[index]);
        case EXTERN_IN:
            return externHandler(&line[index]);
        case ENTRY_IN:
            return entryValidator(&line[index]);
        }
    }
    if (symbol[0])
        addSymbol(&symbols, symbol, ic + RESERVED_MEMORY, CODE);
    // return process_code(line, line_num);
    return process_code(&line[index]);
}

int process_code(char *line)
{
    opcode operation;
    int index = 0;
    addressing_type first_operand;
    addressing_type second_operand;
    int count = 0;
    // figure out operation
    operation = find_operation(line, &index);
    MOVE_TO_NOT_WHITE(line, index);

    // figure out the operands
    if (operation == NONE_OP)
    {
        err = COMMAND_NOT_FOUND;
        return FALSE;
    }
    if (operation == ERROR_OP)
    {

        err = COMMAND_NOT_FOUND;
        return FALSE;
    }

    if (!operationHandler(&line[index], &first_operand, &second_operand))
    {
        return FALSE;
    }

    if (first_operand == ERROR_ADDR || second_operand == ERROR_ADDR)
    {
        return FALSE;
    }
    count += first_operand != NONE_ADDR ? 1 : 0;
    count += second_operand != NONE_ADDR ? 1 : 0;

    if (!validate_operand_count_by_opcode(operation, count))
    {
        err = COMMAND_INVALID_NUMBER_OF_OPERANDS;
        return FALSE;
    }
    if (!command_accept_methods(operation, first_operand, second_operand))
    {
        err = COMMAND_INVALID_ADDRESSING;
        return FALSE;
    }

    insert_instructions(build_first_word(operation, first_operand, second_operand));

    ic += calculate_command_num_additional_words(first_operand, second_operand);

    return TRUE;
}

unsigned int build_first_word(opcode operation, addressing_type first_operand, addressing_type second_operand)
{
    unsigned int word = 0;

    /* Inserting the opcode */
    word = (unsigned int)operation;

    word <<= BITS_IN_ADDRESSING; /* Leave space for first addressing method */
    /* If there are two operands, insert the first */
    if (first_operand != NONE_ADDR && second_operand != NONE_ADDR)
    {
        word |= (unsigned int)first_operand;
    }

    word <<= BITS_IN_ADDRESSING; /* Leave space for second addressing method */

    /* If there are two operands, insert the second. */
    if (first_operand != NONE_ADDR && second_operand != NONE_ADDR)
    {
        word |= (unsigned int)second_operand;
    }
    /* If not, insert the first one (a single operand is a destination operand). */
    else if (first_operand != NONE_ADDR)
    {
        word |= (unsigned int)first_operand;
    }
    word = insert_are(word, ABSOLUTE); /* Insert A/R/E mode to the word */
    // printf("%d %d\n", ic, extract_bits(word, DEST_TYPE_START_POS, DEST_TYPE_END_POS));

    return word;
}

int num_words(addressing_type operand)
{
    switch (operand)
    {
    case IMMEDIATE_ADDR:
    case DIRECT_ADDR:
    case REGISTER_ADDR:
        return 1;
    case INDEX_ADDR:
        return 2;
    }
    return 0;
}

int calculate_command_num_additional_words(addressing_type first_operand, addressing_type second_operand)
{
    int count = 0;
    if (first_operand != NONE_ADDR)
        count += num_words(first_operand);
    if (second_operand != NONE_ADDR)
        count += num_words(second_operand);

    /* If both methods are register, they will share the same additional word */
    if (first_operand == REGISTER_ADDR && second_operand == REGISTER_ADDR)
        count--;
    return count;
}

/* This function checks for the validity of given addressing methods according to the opcode */
int command_accept_methods(opcode type, addressing_type first_operand, addressing_type second_operand)
{
    switch (type)
    {
    /* These opcodes only accept
     * Source: 0, 1, 2, 3
     * Destination: 1, 2, 3
     */
    case MOV_OP:
    case ADD_OP:
    case SUB_OP:
        return (first_operand == IMMEDIATE_ADDR || // 0
                first_operand == DIRECT_ADDR ||    // 1
                first_operand == INDEX_ADDR ||     // 2
                first_operand == REGISTER_ADDR)    // 3
               &&
               (second_operand == DIRECT_ADDR ||
                second_operand == INDEX_ADDR ||
                second_operand == REGISTER_ADDR);

        /* LEA opcode only accept
         * Source: 1, 2
         * Destination: 1, 2, 3
         */
    case LEA_OP:
        return (first_operand == DIRECT_ADDR ||
                first_operand == INDEX_ADDR) &&
               (second_operand == DIRECT_ADDR ||
                second_operand == INDEX_ADDR ||
                second_operand == REGISTER_ADDR);

    /* These opcodes only accept
     * Source: NONE
     * Destination: 1, 2, 3
     */
    case NOT_OP:
    case CLR_OP:
    case INC_OP:
    case DEC_OP:
    case JMP_OP:
    case BNE_OP:
    case RED_OP:
    case JSR_OP:
        return first_operand == DIRECT_ADDR ||
               first_operand == INDEX_ADDR ||
               first_operand == REGISTER_ADDR;

    /* These opcodes are always ok because they accept all methods/none of them and
     * number of operands is being verified in another function
     */
    case PRN_OP:
    case RTS_OP:
    case HLT_OP:
    case CMP_OP:
        return TRUE;
    }
    return FALSE;
}
