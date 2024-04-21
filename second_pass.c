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
    char line[LINESIZE + 1];
    int line_num = 1;
    ic = 0;
    err = FALSE;
    has_error = FALSE;

    while (fgets(line, LINESIZE + 1, fp) != NULL)
    {
        if (!process_line_second_pass(line))
        {
            has_error = TRUE;
            print_error_message(err, line_num);
            // write_error(line_num);
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
        process_operation(operation, &line[index]);
    }
}

int process_operation(opcode operation, char *args)
{
    int index = 0;
    int i = 0, j = 0;
    char first_operand[LINESIZE], second_operand[LINESIZE]; /* will hold first and second operands */
    char *src = first_operand, *dst = second_operand;
    int count = get_operand_count_by_opcode(operation);
    addressing_type src_operand_type = NONE_ADDR, dst_operand_type = NONE_ADDR;
    switch (count)
    {
    case 0:
        ic++;
        return TRUE;
    case 1:
        dst_operand_type = dst_operand_type = extract_bits(instructions[ic], DEST_TYPE_START_POS, DEST_TYPE_END_POS);
        break;
    case 2:
        src_operand_type = extract_bits(instructions[ic], SRC_TYPE_START_POS, SRC_TYPE_END_POS);
        dst_operand_type = dst_operand_type = extract_bits(instructions[ic], DEST_TYPE_START_POS, DEST_TYPE_END_POS);
        break;
    }

    if (count >= 1)
    {
        while (!is_end_of_line(args[index]) && !isspace(args[index]) && args[index] != ',')
        {
            first_operand[i++] = args[index++];
        }
        first_operand[i] = '\0';
        second_operand[0] = '\0';
        dst = first_operand;
        src = NULL;
    }
    if (count == 2)
    {
        index++;
        MOVE_TO_NOT_WHITE(args, index);
        while (!is_end_of_line(args[index]) && !isspace(args[index]) && args[index] != ',')
        {
            second_operand[j++] = args[index++];
        }
        second_operand[j] = '\0';
        src = first_operand;
        dst = second_operand;
    }
    ic++;

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
    unsigned int word = 0; /* An empty word */
    char *tmp1;
    char *tmp2;
    int is_valid = TRUE;

    switch (type)
    {
    case IMMEDIATE_ADDR: /* Extracting immediate number */
        if (is_int_str(&operand[1]))
        {
            word = (unsigned int)atoi(&operand[1]);
            insert_are(word, ABSOLUTE);
            insert_instructions(word);
        }
        else
        {
            is_valid = encode_label(&operand[1]);
        }
        break;

    case DIRECT_ADDR:
        encode_label(operand);
        break;

    case INDEX_ADDR:
        tmp1 = strchr(operand, '[');
        *tmp1 = '\0';
        is_valid = encode_label(operand);
        *tmp1 = '[';
        tmp1++;
        tmp2 = strchr(operand, ']');
        *tmp2 = '\0';
        if (is_int_str(tmp1))
        {
            word = (unsigned int)atoi(tmp1);
            word = insert_are(word, ABSOLUTE);
            insert_instructions(word);
        }
        else
        {
            if (is_valid)
            {
                is_valid = encode_label(tmp1);
            }
            else
            {
                encode_label(tmp1);
            }
        }
        *tmp2 = ']';
        break;

    case REGISTER_ADDR:
        word = build_register_word(is_dst, operand);
        insert_instructions(word);
    }
    return is_valid;
}