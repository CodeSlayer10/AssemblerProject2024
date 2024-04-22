#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "vars.h"
#include "dataHandlers.h"
#include <stdlib.h>

/*
int is_valid_symbol(char symbol);
instruction find_instruction(char *line);
int defineHandler(char *args);
int stringHandler(char *args);
int dataHandler(char *args);
int externHandler(char *args);
*/

instruction find_instruction(char *line, int *index)
{
    int i = 0;
    char tmp_instruction[LINESIZE + 1];
    instruction result;
    MOVE_TO_NOT_WHITE(line, (*index));

    if (is_end_of_line(line[*index]) || line[*index] != '.')
        return NONE_IN;

    *index += find_next_token(&line[*index], tmp_instruction, ' ');
    if ((result = find_instruction_by_name(tmp_instruction)) != NONE_IN)
        return result;
    // not a valid instruction
    err = INSTRUCTION_NOT_FOUND;
    return ERROR_IN;
}

int find_label(char *line, char *symbol)
{
    int index = 0;
    int i = 0;

    MOVE_TO_NOT_WHITE(line, index);
    while (!is_end_of_line(line[index]) && line[index] != ':' && i < SYMBOL_MAX_SIZE)
    {
        symbol[i++] = line[index++];
    }
    symbol[i] = '\0';

    if (line[index] == ':')
    {
        return TRUE;
    }
    if (i == SYMBOL_MAX_SIZE)
    {
        int tmp = index;
        while (!is_end_of_line(line[tmp]) && line[tmp] != ':')
        {
            tmp++;
        }
        if (line[tmp] == ':')
        {
            err = LABEL_TOO_LONG;
            // error: invalid symbol found - should be at most 31 chars
        }
    }

    symbol[0] = '\0';
    return FALSE;
}

int defineHandler(char *arg)
{
    int index = 0;
    int length = 0;
    char symbol[SYMBOL_MAX_SIZE + 1];
    long num_value;

    // remove leading whitespace
    length += find_next_symbol(arg, symbol, '=');
    index += length;

    if (length == SYMBOL_MAX_SIZE)
    {
        err = LABEL_TOO_LONG;
        return FALSE; // too long of a symbol
    }

    if (!is_valid_symbol(symbol))
    {

        return FALSE; // not a valid symbol
    }

    MOVE_TO_NOT_WHITE(arg, index);

    if (arg[index] && arg[index] != '=')
    {
        // symbol with error attribute
        // addSymbol(&symbols, symbol, 0, ERROR);
        err = WARNING_LABEL_ONLY;
        return FALSE; // warning no value specified for [symbol]
    }

    index++;
    MOVE_TO_NOT_WHITE(arg, index);

    char *ptr;
    num_value = strtol(&arg[index], &ptr, 10);

    index += ptr - &arg[index];

    MOVE_TO_NOT_WHITE(arg, index);
    if (arg[index])
    {
        // symbol with error attribute
        // addSymbol(&symbols, symbol, 0, ERROR);
        err = DEFINE_EXPECTED_NUM;
        return FALSE; // expected a number as an argument
    }

    if (!is_in_range(num_value))
    {
        // addSymbol(&symbols, symbol, 0, ERROR);
        err = NUM_OUT_OF_RANGE;

        // symbol with error attribute
        return FALSE;
    }

    if (locateSymbol(&symbols, symbol))
    {
        err = LABEL_ALREADY_EXISTS;
        return FALSE;
    }
    addSymbol(&symbols, symbol, (int)num_value, MDEFINE);

    return TRUE;
}

int dataHandler(char *args)
{
    int index = 0;
    char arg[SYMBOL_MAX_SIZE + 1];
    char *rest;
    long value;
    Symbol *symbol;
    MOVE_TO_NOT_WHITE(args, index);

    while (!is_end_of_line(args[index]))
    {
        if (args[index] == ',')
        {
            err = DATA_UNEXPECTED_COMMA;
            // unexpected , character
            return FALSE;
        }

        value = strtol(&args[index], &rest, 10);

        if (rest == &args[index])
        {
            index += find_next_symbol(&args[index], arg, ',');

            if (!is_valid_symbol(arg))
            {
                return FALSE;
            }
            symbol = findSymbol(&symbols, arg);
            if (symbol == NULL)
            {
                err = DATA_LABEL_DOES_NOT_EXIST;

                return FALSE;
            }
            
            if (symbol->attribute != MDEFINE)
            {
                err = DATA_EXPECTED_CONST;
                return FALSE;
            }
            insert_data(symbol->value);
        }
        else
        {
            index += (rest - (args + index));
            // check if in range of num
            if (!is_in_range(value))
            {
                err = NUM_OUT_OF_RANGE;
                // symbol with error attribute
                return FALSE;
            }
            insert_data((int)value);
        }
        MOVE_TO_NOT_WHITE(args, index);
        if (args[index] == ',')
        {
            index++; // Skip over the comma
            MOVE_TO_NOT_WHITE(args, index);
            if (is_end_of_line(args[index]))
            {
                err = DATA_UNEXPECTED_COMMA;
                return FALSE; // Trailing ',' at the end
            }
        }
        else
        {
            if (!is_end_of_line(args[index]))
            {
                err = DATA_EXPECTED_COMMA_AFTER_NUM;
                return FALSE;
            }
        }
    }
    return TRUE;
}

int stringHandler(char *args)
{
    int index = 0;
    MOVE_TO_NOT_WHITE(args, index);

    if (args[index] != '"')
    {
        // pos = index;
        err = STRING_OPERAND_NOT_VALID;
        return FALSE;
    }
    char *ptr = strrchr(&args[index], '"');
    index++;

    while (args[index] && isprint(args[index]) && &args[index] != ptr)
    {
        insert_data((int)args[index++]);
    }
    insert_data('\0');

    if (args[index] != '"')
    {
        // pos = index;
        err = STRING_OPERAND_NOT_VALID;
        return FALSE;
    }
    index++;

    MOVE_TO_NOT_WHITE(args, index);
    if (!is_end_of_line(args[index]))
    {
        // pos = index;
        err = STRING_UNEXPECTED_CHARS;
        return FALSE;
    }
    return TRUE;
}

int entryValidator(char *arg)
{
    int index = 0;
    char symbol[SYMBOL_MAX_SIZE + 1];

    index += find_next_symbol(&arg[index], symbol, ',');
    if (!is_valid_symbol(symbol))
    {
        return FALSE;
    }
    if (locateSymbol_by_attribute(&symbols, symbol, EXTERNAL))
    {
        err = ENTRY_CANT_BE_EXTERN;
        return FALSE;
    }

    MOVE_TO_NOT_WHITE(arg, index);

    if (!is_end_of_line(arg[index]))
    {
        err = ENTRY_TOO_MANY_OPERANDS;
        return FALSE;
    }

    return TRUE;
}

int entryHandler(char *arg)
{
    int index = 0;
    char symbol[SYMBOL_MAX_SIZE + 1];

    int i = 0;
    index += find_next_symbol(&arg[index], symbol, ',');

    if (!change_to_entry(&symbols, symbol))
    {
        err = ENTRY_LABEL_DOES_NOT_EXIST;
        return FALSE;
    }
    has_entry = TRUE;
    return TRUE;
}

int externHandler(char *arg)
{
    int index = 0;
    char symbol[SYMBOL_MAX_SIZE + 1];

    index += find_next_symbol(&arg[index], symbol, ',');

    if (!is_valid_symbol(symbol))
    {
        return FALSE;
    }
    if (locateSymbol(&symbols, symbol))
    {
        err = LABEL_ALREADY_EXISTS;
        return FALSE;
    }

    MOVE_TO_NOT_WHITE(arg, index);

    if (!is_end_of_line(arg[index]))
    {
        err = ENTRY_TOO_MANY_OPERANDS;
        return FALSE;
    }
    has_external = TRUE;
    addSymbol(&symbols, symbol, 0, EXTERNAL);

    return TRUE;
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
