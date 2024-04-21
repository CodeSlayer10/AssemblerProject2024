#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vars.h"
#include "cmdHandlers.h"

opcode find_operation(char *line, int *index)
{
    char tmp_operation[LINESIZE + 1];
    opcode result;
    MOVE_TO_NOT_WHITE(line, *index);
    if (is_end_of_line(line[*index]))
        return NONE_OP;
    *index += find_next_token(&line[*index], tmp_operation, ' ');
    if ((result = find_operation_by_name(tmp_operation)) != NONE_OP)
        return result;
    // not a valid instruction
    return ERROR_OP;
}

addressing_type get_addressing_type(char *operand)
{
    char *opening;
    char *closing;
    int tmp_err = err;
    if (operand[0] == '\0')
    {
        return NONE_ADDR;
    }
    if (operand[0] == '#' && (is_int_str(&operand[1]) || (is_valid_symbol(&operand[1]) && locateSymbol_by_attribute(&symbols, &operand[1], MDEFINE))))
    {
        return IMMEDIATE_ADDR;
    }


    if (find_register_by_name(operand) != R_NONE)
    {
        return REGISTER_ADDR;
    }

    if (is_valid_symbol(operand) )
    {
        return DIRECT_ADDR;
    }

    

    if ((opening = strchr(operand, '[')) == NULL)
    {
        err = INVALID_ADDRESSING_TYPE;
        return ERROR_ADDR;
    }
    *opening = '\0';
    int is_symbol = is_valid_symbol(operand);
    *opening = '[';
    if (!is_symbol)
    {
        return ERROR_ADDR;
    }
    if ((closing = strrchr(operand, ']')) == NULL)
    {

        err = INDEX_EXPECTED_CLOSING_BRACKET;
        return ERROR_ADDR;
    }
    opening++;
    *closing = '\0';

    int is_valid = (is_int_str(opening) || (is_valid_symbol(opening) && locateSymbol_by_attribute(&symbols, opening, MDEFINE)));
    *closing = ']';
    if (!is_valid)
    {
        err = INDEX_INVALID_POSITION;
        return ERROR_ADDR;
    }
    closing++;

    if (!is_end_of_line(*closing))
    {

        err = COMMAND_UNEXPECTED_CHAR;
        return ERROR_ADDR;
    }
    err = tmp_err;
    return INDEX_ADDR;
}

int operationHandler(char *line, addressing_type *first_operand, addressing_type *second_operand)
{
    int index = 0;
    char operand1[LINESIZE + 1], operand2[LINESIZE + 1];

    // Extract first operand
    MOVE_TO_NOT_WHITE(line, index);
    if (line[index] == ',')
    {
        err = COMMAND_UNEXPECTED_CHAR;
        return FALSE;
    }

    index += find_next_token(&line[index], operand1, ',');

    *first_operand = get_addressing_type(operand1);

    // Check for second operand
    MOVE_TO_NOT_WHITE(line, index);
    if (line[index] == ',')
    {

        index++;
        MOVE_TO_NOT_WHITE(line, index);
        if (is_end_of_line(line[index]))
        {
            err = COMMAND_UNEXPECTED_CHAR; // Redundant comma at the end
            return FALSE;
        }
        index += find_next_token(&line[index], operand2, ',');

        *second_operand = get_addressing_type(operand2);
    }
    else if (!is_end_of_line(line[index]))
    {

        err = EXPECTED_COMMA_BETWEEN_OPERANDS; // Unexpected characters after first operand
        return FALSE;
    }
    else
    {
        *second_operand = NONE_ADDR; // Only one operand
    }

    MOVE_TO_NOT_WHITE(line, index);
    if (!is_end_of_line(line[index]))
    {
        err = COMMAND_UNEXPECTED_CHAR;
        return FALSE;
    }

    return TRUE;
}
