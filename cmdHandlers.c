#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vars.h"
#include "cmdHandlers.h"

/**
 * Finds the operation opcode in a line of assembly code.
 * @param line The line of assembly code.
 * @param index Pointer to the index indicating the position in the line.
 * @return The opcode of the operation found, or NONE_OP if not found.
 */
opcode find_operation(char *line, int *index)
{
    char tmp_operation[LINESIZE + 1]; // Temporary buffer to hold the operation name
    opcode result;                    // Variable to store the result

    MOVE_TO_NOT_WHITE(line, *index); // Move index to the first non-white space character

    if (is_end_of_line(line[*index]))
        return NONE_OP; // No operation found if end of line is reached

    *index += find_next_token(&line[*index], tmp_operation, ' '); // Find the next token in the line

    if ((result = find_operation_by_name(tmp_operation)) != NONE_OP)
        return result; // Return the operation opcode if found

    return NONE_OP; // Return NONE_OP if not a valid operation
}

addressing_type get_addressing_type(char *operand)
{
    char *opening;
    char *closing;
    int is_symbol;
    int is_valid;
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

    if (is_valid_symbol(operand))
    {
        return DIRECT_ADDR;
    }

    if ((opening = strchr(operand, '[')) == NULL)
    {
        err = INVALID_ADDRESSING_TYPE;
        return ERROR_ADDR;
    }
    *opening = '\0';
    is_symbol = is_valid_symbol(operand);
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

    is_valid = (is_int_str(opening) || (is_valid_symbol(opening) && locateSymbol_by_attribute(&symbols, opening, MDEFINE)));
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
    err = FALSE;
    return INDEX_ADDR;
}

/**
 * Handles the operations and extracts operands from a line of assembly code.
 * @param line The line of assembly code.
 * @param first_operand Pointer to store the addressing type of the first operand.
 * @param second_operand Pointer to store the addressing type of the second operand.
 * @return TRUE if the operation is successfully handled and operands are extracted, FALSE otherwise.
 */
int operationHandler(char *line, addressing_type *first_operand, addressing_type *second_operand)
{
    int index = 0;                                       // Initialize index to track position in the line
    char operand1[LINESIZE + 1], operand2[LINESIZE + 1]; // Temporary buffers to hold operands

    MOVE_TO_NOT_WHITE(line, index); // Move index to the first non-white space character
    if (line[index] == ',')
    {
        err = COMMAND_UNEXPECTED_CHAR; // Error handling: set error if unexpected comma at the beginning
        return FALSE;                  // Return FALSE indicating unexpected comma at the beginning
    }

    // Find and extract the first operand
    index += find_next_token(&line[index], operand1, ','); // Find and extract the first operand
    *first_operand = get_addressing_type(operand1);        // Get addressing type of the first operand
    if (*first_operand == ERROR_ADDR)
    {
        return FALSE; // Return FALSE indicating an error in the first operand
    }

    // Check for a second operand
    MOVE_TO_NOT_WHITE(line, index); // Move index to the next non-white space character
    if (line[index] == ',')
    {
        index++; // Move past the comma

        // Check for trailing comma
        MOVE_TO_NOT_WHITE(line, index); // Move index to the next non-white space character
        if (is_end_of_line(line[index]))
        {
            err = COMMAND_UNEXPECTED_CHAR; // Error handling: set error if redundant comma at the end
            return FALSE;                  // Return FALSE indicating redundant comma at the end
        }

        // Find and extract the second operand
        index += find_next_token(&line[index], operand2, ','); // Find and extract the second operand
        *second_operand = get_addressing_type(operand2);       // Get addressing type of the second operand
    }
    else if (!is_end_of_line(line[index]))
    {
        err = EXPECTED_COMMA_BETWEEN_OPERANDS; // Error handling: set error if unexpected characters after first operand
        return FALSE;                          // Return FALSE indicating unexpected characters after first operand
    }
    else
    {
        *second_operand = NONE_ADDR; // Only one operand
    }

    // Check for unexpected characters after the operands
    MOVE_TO_NOT_WHITE(line, index); // Move index to the next non-white space character
    if (!is_end_of_line(line[index]))
    {
        err = COMMAND_UNEXPECTED_CHAR; // Error handling: set error if unexpected characters after the operands
        return FALSE;                  // Return FALSE indicating unexpected characters after the operands
    }

    return TRUE; // Return TRUE indicating successful handling of operation and extraction of operands
}
