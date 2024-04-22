#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "dataHandlers.h"
#include "cmdHandlers.h"
#include "vars.h"
#include "second_pass.h"

void second_pass(FILE *fp)
{
    char line[LINESIZE + 2];
    int line_num = 1;
    ic = 0;
    has_error = FALSE;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        err = FALSE;
        warn = FALSE;
        if (!process_line_second_pass(line))
        {
            has_error = TRUE;
            print_error_message(err, line_num);
            // write_error(line_num);
        }
        if (warn)
        {
            print_error_message(warn, line_num);
        }
        line_num++;
    }
}

int process_line_second_pass(char *line)
{
    instruction instruction;
    opcode operation;
    char symbol[SYMBOL_MAX_SIZE + 1];
    int index = 0;
    MOVE_TO_NOT_WHITE(line, index);

    find_label(line, symbol);

    if (symbol[0])
    {
        for (; line[index] != ':'; index++)
            ;
        index++;
    }

    MOVE_TO_NOT_WHITE(line, index);

    if (is_end_of_line(line[index]))
    {
        return TRUE;
    }

    instruction = find_instruction(line, &index);
    MOVE_TO_NOT_WHITE(line, index);
    if (instruction != NONE_IN)
    {
        if (instruction == ENTRY_IN)
        {
            return entryHandler(&line[index]);
        }
        return TRUE;
    }
    operation = find_operation(line, &index);
    MOVE_TO_NOT_WHITE(line, index);
    if (operation != NONE_OP)
    {
        return process_operation(operation, &line[index]);
    }
}

int process_operation(opcode operation, char *args)
{
    int index = 0;
    char first_operand[LINESIZE], second_operand[LINESIZE];                     // Hold operands
    char *src = NULL, *dst = NULL;                                              // Pointers to operands
    int count = get_operand_count_by_opcode(operation);                         // Number of operands
    addressing_type src_operand_type = NONE_ADDR, dst_operand_type = NONE_ADDR; // Operand types

    // Extract operand types based on the count of operands
    if (count == 1 || count == 2)
    {
        dst_operand_type = extract_bits(instructions[ic], DEST_TYPE_START_POS, DEST_TYPE_END_POS);
    }
    if (count == 2)
    {
        src_operand_type = extract_bits(instructions[ic], SRC_TYPE_START_POS, SRC_TYPE_END_POS);
    }

    // Extract operands from the argument string
    index += find_next_symbol(args, first_operand, ',');
    index++;
    index += find_next_symbol(&args[index], second_operand, ',');

    // Set src and dst pointers based on the number of operands
    if (count >= 1)
    {
        dst = first_operand; // First operand is destination
    }
    if (count == 2)
    {
        src = first_operand;  // Second operand is source
        dst = second_operand; // First operand becomes destination
    }

    ic++; // Increment instruction count

    // Encode additional words based on operands and their types
    return encode_additional_words(src, dst, src_operand_type, dst_operand_type);
}

int encode_additional_words(char *src_operand, char *dst_operand, addressing_type src_type, addressing_type dst_type)
{
    int is_valid = TRUE;
    if (src_type == REGISTER_ADDR && dst_type == REGISTER_ADDR)
    {
        insert_instructions(build_register_word(FALSE, src_operand) | build_register_word(TRUE, dst_operand));
    }
    else
    {
        if (src_operand)
        {
            is_valid = encode_additional_word(FALSE, src_type, src_operand);
        }
        if (dst_operand)
        {
            is_valid = encode_additional_word(TRUE, dst_type, dst_operand);
        }
    }
    return is_valid;
}

unsigned int build_register_word(int is_dst, char *reg)
{
    unsigned int word = (unsigned int)atoi(reg + 1); /* Getting the register's number */
    /* Inserting it to the required bits (by source or destination operand) */
    if (!is_dst)
        word <<= BITS_IN_REGISTER;
    word = insert_are(word, ABSOLUTE);
    return word;
}

int encode_label(char *symbol)
{
    unsigned int word = 0;
    Symbol *symbol_info = findSymbol(&symbols, symbol);
    if (symbol_info == NULL)
    {
        ic++;
        err = COMMAND_LABEL_DOES_NOT_EXIST;
        return FALSE;
    }
    word = (unsigned int)symbol_info->value;
    switch (symbol_info->attribute)
    {
    case MDEFINE:
        word = insert_are(word, ABSOLUTE);
        break;
    case EXTERNAL:
        add_ext(&externals, symbol, ic);
        word = insert_are(word, EXTERN);
        break;

    default:
        word = insert_are(word, RELOCATABLE);
    }
    insert_instructions(word);
    return TRUE;
}
int encode_additional_word(int is_dst, addressing_type type, char *operand)
{
    unsigned int word = 0;
    int is_valid = TRUE;

    switch (type)
    {
    case IMMEDIATE_ADDR:
        is_valid = handle_immediate_address(operand, &word);
        break;

    case DIRECT_ADDR:
        encode_label(operand);
        break;

    case INDEX_ADDR:
        is_valid = handle_index_address(operand, &word);
        break;

    case REGISTER_ADDR:
        word = build_register_word(is_dst, operand);
        insert_instructions(word);
        break;
    }

    return is_valid;
}

int handle_immediate_address(char *operand, unsigned int *word)
{
    if (is_int_str(&operand[1]))
    {
        *word = (unsigned int)atoi(&operand[1]);
        insert_are(*word, ABSOLUTE);
        insert_instructions(*word);
        return TRUE;
    }
    else
    {
        return encode_label(&operand[1]);
    }
}

int handle_index_address(char *operand, unsigned int *word)
{
    char *opening_bracket;
    char *closing_bracket;
    int is_valid;
    opening_bracket = strchr(operand, '[');
    closing_bracket = strchr(operand, ']');
    *opening_bracket = '\0';
    is_valid = encode_label(operand);
    *opening_bracket = '[';
    opening_bracket++;
    *closing_bracket = '\0';
    if (is_int_str(opening_bracket))
    {
        *word = (unsigned int)atoi(opening_bracket);
        *word = insert_are(*word, ABSOLUTE);
        insert_instructions(*word);
    }
    else
    {
        if (is_valid)
        {
            is_valid = encode_label(opening_bracket);
        }
        else
        {
            encode_label(opening_bracket);
        }
    }
    *closing_bracket = ']';
    return is_valid;
}