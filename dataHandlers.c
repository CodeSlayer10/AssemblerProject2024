#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vars.h"
#include "dataHandlers.h"
#include <stdlib.h>

/**
 * Finds the instruction in a line of assembly code.
 * @param line The line of assembly code.
 * @param index Pointer to the index indicating the position in the line.
 * @return The instruction found, or NONE_IN if not found.
 */
instruction find_instruction(char *line, int *index)
{
    // Temporary buffer to store the instruction
    char tmp_instruction[LINESIZE + 1];
    // Resulting instruction
    instruction result;

    // Moves to the start of the line
    MOVE_TO_NOT_WHITE(line, (*index));

    // Checks if the line is empty or if the first arg doesn't start with a (.)
    if (is_end_of_line(line[*index]) || line[*index] != '.')
        return NONE_IN; // Return NONE_IN if end of line or not starting with a (.)

    // Moves to and saves the next token from the current position
    *index += find_next_token(&line[*index], tmp_instruction, ' '); // Find and save the next token

    // Checks if the current token is an instruction
    if ((result = find_instruction_by_name(tmp_instruction)) != NONE_IN)
        return result; // Return the instruction if found

    // If instruction not found, set error and return
    err = INSTRUCTION_NOT_FOUND; // Error handling: set error if instruction not found
    return ERROR_IN;             // Return ERROR_IN indicating an error
}

/**
 * Finds a label in a line of assembly code.
 * @param line The line of assembly code.
 * @param symbol Pointer to store the found symbol.
 * @return TRUE if a label definition is found, FALSE otherwise.
 */
int find_label(char *line, char *symbol)
{
    int index = 0; // Initialize index to track position in the line
    int i = 0;     // Initialize counter for symbol characters

    // Moves to the start of the line
    MOVE_TO_NOT_WHITE(line, index);

    // Iterates through the line until end of line, colon (:) or symbol size limit is reached
    while (!is_end_of_line(line[index]) && line[index] != ':' && i < SYMBOL_MAX_SIZE)
    {
        symbol[i++] = line[index++]; // Copy characters to symbol buffer
    }
    // Null terminate the symbol string
    symbol[i] = '\0';

    // Checks if a colon (:) is found, indicating a label definition
    if (line[index] == ':')
    {
        return TRUE; // Return TRUE if label definition found
    }

    // Checks if symbol size limit is reached
    if (i == SYMBOL_MAX_SIZE)
    {
        int tmp = index; // Temporary variable to track position in the line
        // Move tmp to the end of the line or until a colon (:) is found
        while (!is_end_of_line(line[tmp]) && line[tmp] != ':')
        {
            tmp++;
        }
        // If a colon (:) is found, set error for label being too long
        if (line[tmp] == ':')
        {
            err = LABEL_TOO_LONG; // Error handling: set error if label is too long
        }
    }

    // Reset symbol to empty string
    symbol[0] = '\0';
    return FALSE; // Return FALSE if label definition not found
}

/**
 * Handles the definition of a constant.
 * @param arg The argument string containing the constant definition.
 * @return TRUE if the constant is successfully defined, FALSE otherwise.
 */
int defineHandler(char *arg)
{
    int index = 0;                    // Initialize index to track position in the argument string
    int length = 0;                   // Initialize length to track length of symbol
    char symbol[SYMBOL_MAX_SIZE + 1]; // Buffer to store the symbol
    long num_value;                   // Variable to store the numeric value

    // Removes leading whitespace and finds the next symbol
    length += find_next_symbol(arg, symbol, '='); // Find and extract the symbol from the argument
    index += length;                              // Update index

    // Checks if symbol length exceeds the maximum allowed size
    if (length == SYMBOL_MAX_SIZE)
    {
        err = LABEL_TOO_LONG; // Error handling: set error if symbol is too long
        return FALSE;         // Return FALSE indicating symbol is too long
    }

    // Checks if symbol is valid
    if (!is_valid_symbol(symbol))
    {
        return FALSE; // Return FALSE indicating symbol is not valid
    }

    // Moves index to the next non-space character
    MOVE_TO_NOT_WHITE(arg, index);

    // Checks if the line is empty after symbol extraction
    if (is_end_of_line(arg[index]))
    {
        warn = WARNING_EMPTY_LABEL; // Warning: set warning if label is empty
        return TRUE;                // Return TRUE indicating empty label
    }

    // Checks if there's an equal sign after the symbol
    if (arg[index] && arg[index] != '=')
    {
        err = DEFINE_EXPECTED_EQUAL; // Error handling: set error if equal sign is expected
        return FALSE;                // Return FALSE indicating equal sign expected
    }
    index++; // Move past the equal sign

    // Moves index to the next non-white space character
    MOVE_TO_NOT_WHITE(arg, index);

    // Checks if the line is empty after the equal sign
    if (is_end_of_line(arg[index]))
    {
        err = DEFINE_EXPECTED_NUM; // Error handling: set error if number is expected
        return FALSE;              // Return FALSE indicating number expected
    }

    // Converts string argument to a long integer
    char *ptr;
    num_value = strtol(&arg[index], &ptr, 10); // Convert string to long integer
    index += ptr - &arg[index];                // Update index

    // Moves to the next non-white space character
    MOVE_TO_NOT_WHITE(arg, index);

    // Check if there are any characters left on the line after the number
    if (!is_end_of_line(arg[index]))
    {
        err = DEFINE_EXPECTED_NUM; // Error handling: set error if number is expected
        return FALSE;              // Return FALSE indicating number expected
    }

    // Checks if the number is within the allowed range
    if (!is_in_range(num_value))
    {
        err = NUM_OUT_OF_RANGE; // Error handling: set error if number is out of range
        return FALSE;           // Return FALSE indicating number is out of range
    }

    // Checks if the symbol already exists
    if (locateSymbol(&symbols, symbol))
    {
        err = LABEL_ALREADY_EXISTS; // Error handling: set error if label already exists
        return FALSE;               // Return FALSE indicating label already exists
    }

    // Add symbol to the symbol table
    addSymbol(&symbols, symbol, (int)num_value, MDEFINE); // Add symbol to symbol table with numeric value

    return TRUE; // Return TRUE indicating constant successfully defined
}

/**
 * Handles the .data instruction.
 * @param args The argument string containing the data values.
 * @return TRUE if the data values are successfully processed, FALSE otherwise.
 */
int dataHandler(char *args)
{
    int index = 0;                 // Initialize index to track position in the argument string
    char arg[SYMBOL_MAX_SIZE + 1]; // Buffer to store symbols or numeric values
    char *rest;                    // Pointer to the rest of the string after conversion
    long value;                    // Variable to store numeric value
    Symbol *symbol;                // Pointer to symbol information

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(args, index);

    // Iterate through the arguments until end of line
    while (!is_end_of_line(args[index]))
    {
        // Check for unexpected comma
        if (args[index] == ',')
        {
            err = DATA_UNEXPECTED_COMMA; // Error handling: set error if unexpected comma found
            return FALSE;                // Return FALSE indicating unexpected comma
        }

        // Convert string to long integer
        value = strtol(&args[index], &rest, 10);

        // If rest points to the same location as index, it means no numeric value was found
        if (rest == &args[index])
        {
            // Find next symbol
            index += find_next_symbol(&args[index], arg, ',');

            // Check if symbol is valid
            if (!is_valid_symbol(arg))
            {
                return FALSE; // Return FALSE indicating symbol is not valid
            }

            // Find symbol in the symbol table
            symbol = findSymbol(&symbols, arg);
            if (symbol == NULL)
            {
                err = DATA_LABEL_DOES_NOT_EXIST; // Error handling: set error if symbol does not exist
                return FALSE;                    // Return FALSE indicating symbol does not exist
            }

            // Check if the symbol is a constant
            if (symbol->attribute != MDEFINE)
            {
                err = DATA_EXPECTED_CONST; // Error handling: set error if expected constant value
                return FALSE;              // Return FALSE indicating expected constant value
            }

            // Insert data from the symbol's value
            insert_data(symbol->value);
        }
        else
        {
            // Move index by the difference between rest and the start of args
            index += (rest - (args + index));

            // Check if the numeric value is within range
            if (!is_in_range(value))
            {
                err = NUM_OUT_OF_RANGE; // Error handling: set error if number out of range
                return FALSE;           // Return FALSE indicating number out of range
            }

            // Insert the numeric value as data
            insert_data((int)value);
        }

        // Move index to the next non-white space character
        MOVE_TO_NOT_WHITE(args, index);

        // Check for a comma and handle accordingly
        if (args[index] == ',')
        {
            index++; // Skip over the comma
            MOVE_TO_NOT_WHITE(args, index);

            // Check for trailing comma at the end
            if (is_end_of_line(args[index]))
            {
                err = DATA_UNEXPECTED_COMMA; // Error handling: set error if trailing comma found
                return FALSE;                // Return FALSE indicating trailing comma found
            }
        }
        else
        {
            // If no comma is found, check if it's the end of the line
            if (!is_end_of_line(args[index]))
            {
                err = DATA_EXPECTED_COMMA_AFTER_NUM; // Error handling: set error if expected comma after numeric value
                return FALSE;                        // Return FALSE indicating expected comma after numeric value
            }
        }
    }

    return TRUE; // Return TRUE indicating successful handling of .data instruction
}

/**
 * Handles the .string instruction.
 * @param args The argument string containing the string value.
 * @return TRUE if the string value is successfully processed, FALSE otherwise.
 */
int stringHandler(char *args)
{
    int index = 0; // Initialize index to track position in the argument string

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(args, index);

    // Check if the first character is a double quote (")
    if (args[index] != '"')
    {
        err = STRING_OPERAND_NOT_VALID;
        return FALSE; // Return FALSE indicating string operand not valid
    }

    // Find the last occurrence of double quote (") in the string
    char *ptr = strrchr(&args[index], '"');
    index++;

    // Iterate through the characters of the string until the last double quote
    while (args[index] && isprint(args[index]) && &args[index] != ptr)
    {
        // Insert each character into data
        insert_data((int)args[index++]);
    }

    // Insert null terminator for the string
    insert_data('\0');

    // Check if the last character is a double quote (")
    if (args[index] != '"')
    {
        err = STRING_OPERAND_NOT_VALID;
        return FALSE; // Return FALSE indicating string operand not valid
    }
    index++;

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(args, index);

    // Check if there are any characters left on the line after the string
    if (!is_end_of_line(args[index]))
    {
        err = STRING_UNEXPECTED_CHARS;
        return FALSE; // Return FALSE indicating unexpected characters after string
    }

    return TRUE; // Return TRUE indicating successful parsing of string
}

/**
 * Validates the .entry directive.
 * @param arg The argument string containing the entry symbol.
 * @return TRUE if the entry symbol is successfully validated, FALSE otherwise.
 */
int entryValidator(char *arg)
{
    int index = 0;                    // Initialize index to track position in the argument string
    char symbol[SYMBOL_MAX_SIZE + 1]; // Buffer to store the entry symbol

    // Find the next symbol in the argument
    index += find_next_symbol(&arg[index], symbol, ',');

    // Check if the symbol is valid
    if (!is_valid_symbol(symbol))
    {
        return FALSE; // Return FALSE indicating invalid symbol
    }

    // Check if the symbol is declared as external
    if (locateSymbol_by_attribute(&symbols, symbol, EXTERNAL))
    {
        err = ENTRY_CANT_BE_EXTERN;
        return FALSE; // Return FALSE indicating entry symbol cannot be external
    }

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(arg, index);

    // Check if there are any characters left on the line after the symbol
    if (!is_end_of_line(arg[index]))
    {
        err = ENTRY_TOO_MANY_OPERANDS;
        return FALSE; // Return FALSE indicating too many operands for entry directive
    }

    return TRUE; // Return TRUE indicating successful validation of entry symbol
}

/**
 * Handles the .entry directive.
 * @param arg The argument string containing the entry symbol.
 * @return TRUE if the entry symbol is successfully handled, FALSE otherwise.
 */
int entryHandler(char *arg)
{
    int index = 0;                    // Initialize index to track position in the argument string
    char symbol[SYMBOL_MAX_SIZE + 1]; // Buffer to store the entry symbol

    // Find the next symbol in the argument
    index += find_next_symbol(&arg[index], symbol, ',');

    // Change the symbol's attribute to entry in the symbol table
    if (!change_to_entry(&symbols, symbol))
    {
        err = ENTRY_LABEL_DOES_NOT_EXIST;
        return FALSE; // Return FALSE indicating entry label does not exist
    }

    // Set flag indicating that an entry symbol exists
    has_entry = TRUE;

    return TRUE; // Return TRUE indicating successful handling of entry symbol
}

/**
 * Handles the .extern directive.
 * @param arg The argument string containing the external symbol.
 * @return TRUE if the external symbol is successfully handled, FALSE otherwise.
 */
int externHandler(char *arg)
{
    int index = 0;                    // Initialize index to track position in the argument string
    char symbol[SYMBOL_MAX_SIZE + 1]; // Buffer to store the external symbol

    // Find the next symbol in the argument
    index += find_next_symbol(&arg[index], symbol, ',');

    // Check if the symbol is valid
    if (!is_valid_symbol(symbol))
    {
        return FALSE; // Return FALSE indicating invalid symbol
    }

    // Check if the symbol already exists in the symbol table
    if (locateSymbol(&symbols, symbol))
    {
        err = LABEL_ALREADY_EXISTS;
        return FALSE; // Return FALSE indicating label already exists
    }

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(arg, index);

    // Check if there are any characters left on the line after the symbol
    if (!is_end_of_line(arg[index]))
    {
        err = ENTRY_TOO_MANY_OPERANDS;
        return FALSE; // Return FALSE indicating too many operands for extern directive
    }

    // Set flag indicating that an external symbol exists
    has_external = TRUE;

    // Add the symbol to the symbol table with attribute EXTERNAL and value 0
    addSymbol(&symbols, symbol, 0, EXTERNAL);

    return TRUE; // Return TRUE indicating successful handling of extern symbol
}

// int externHandler(char *args)
// {
//     int index = 0;
//     char symbol[SYMBOL_MAX_SIZE + 1];
//     int isValid = TRUE;

//     while (!is_end_of_line(args[index]))
//     {
//         int i = 0;
//         MOVE_TO_NOT_WHITE(args, index);

//         if (args[index] == ',')
//         {
//             // unexpected , character
//             isValid = FALSE;
//             index++;
//         }

//         // Copy symbol
//         while (!is_end_of_line(args[index]) && !isspace(args[index]) && args[index] != ',' && i < SYMBOL_MAX_SIZE)
//         {
//             symbol[i++] = args[index++];
//         }
//         symbol[i] = '\0';

//         // Move to the next char
//         MOVE_TO_NOT_WHITE(args, index);

//         // Check if there an item after there is also a comma between.
//         if (args[index] && args[index] != ',')
//         {
//             isValid = FALSE;
//         }
//         // Check if symbol is too long or not valid
//         if (!is_valid_symbol(symbol))
//         {
//             isValid = FALSE;
//             continue; // Move to the next symbol
//         }
//         if (findSymbol(symbol) != NULL)
//         {
//             isValid = FALSE;
//             continue; // Move to the next symbol
//         }
//         // Add valid symbol
//         addSymbol(symbol, 0, EXTERNAL);
//     }
//     return isValid;
// }
