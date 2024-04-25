#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "utils.h"
#include "preAssembler.h"
#include "vars.h"
#include "hashtable.h"

/**
 * Pre-processes each line from the input file, preparing it for assembly.
 * Checks for line length and expands macro's.
 * @param file Pointer to the input file.
 * @param fp Pointer to the output file.
 */
void preAssembler(FILE *file, FILE *fp)
{
    char line[LINESIZE + 2]; // Buffer to hold each line read from the file
    char macro[SYMBOL_MAX_SIZE + 1]; // Buffer to hold macro definitions
    unsigned char index = 0; // Index for macro buffer
    int line_num = 1; // Line number counter

    // Loop through each line in the file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        err = FALSE; // Reset error flag for each line
        warn = FALSE; // Reset warning flag for each line

        // Check if line length exceeds maximum and fix if necessary
        if (strlen(line) == LINESIZE + 1 && line[LINESIZE] != '\n')
        {
            warn = WARNING_LINE_TOO_LONG; // Set warning flag for long lines
            print_error_message(warn, line_num); // Print warning message

            // Flush the rest of the line to avoid processing remnants
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF)
                ;
            line[LINESIZE] = '\n'; // Add newline character at correct position
            line[LINESIZE + 1] = '\0'; // Truncate line to LINESIZE characters
        }

        // Pre-process the current line and check for errors
        if (!pre_process_line(line, fp, macro))
        {
            has_error = TRUE; // Set overall error flag if an error occurs
            print_error_message(err, line_num); // Print error message
        }

        line_num++; // Increment line number
    }
}


/**
 * Pre_processes a single line of code, handling macros and checking for errors.
 * @param line Pointer to the line of code to be pre_processed.
 * @param fp Pointer to the output file.
 * @param macro Pointer to the buffer for macro definitions.
 * @return Returns TRUE if the line was processed successfully, FALSE otherwise.
 */
int pre_process_line(char *line, FILE *fp, char *macro)
{
    int index = 0; // Index for traversing the line
    char field[SYMBOL_MAX_SIZE + 1]; // Buffer for storing symbols

    MOVE_TO_NOT_WHITE(line, index); // Move index to the first non-white character

    // Check for end of line or comment
    if (is_end_of_line(line[index]) || line[index] == ';')
        return TRUE; // Skip empty lines or comments

    // Extract the next symbol from the line
    int length = find_next_symbol(&line[index], field, ' ');
    index += length; // Move index forward by the length of the symbol

    // Handle long symbols
    if (length == SYMBOL_MAX_SIZE)
        field[0] = '\0'; // Empty the field to indicate an invalid symbol

    // Look up the symbol in the macro table
    node *tmp = lookup(macroTable, field);

    MOVE_TO_NOT_WHITE(line, index); // Move index to the next non-white character

    // If the symbol is a macro, expand it
    if (tmp != NULL)
    {
        if (!is_end_of_line(line[index])) // Check for unexpected characters after the macro
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE; // Return false to indicate error
        }
        // Expand the macro by writing its lines to the output file
        for (node *current = tmp; current != NULL; current = current->next)
            fputs(current->line, fp);
        return TRUE; // Return true to indicate successful processing
    }

    // Check if the symbol is "mcr" (macro definition)
    if (strcmp(field, "mcr") == 0)
    {
        // Extract the macro name and move index accordingly
        length = find_next_symbol(&line[index], macro, ' ');
        index += length;

        // Handle long macro names
        if (length == SYMBOL_MAX_SIZE)
            field[0] = '\0'; // Empty the field to indicate an invalid macro name

        MOVE_TO_NOT_WHITE(line, index); // Move index to the next non-white character

        // Check for unexpected characters after the macro name or if the macro name is invalid
        if (!is_end_of_line(line[index]) || (macro[0] && !is_valid_macro(macro)))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE; // Return false to indicate error
        }
    }
    // Check if the symbol is "endmcr" (end of macro definition)
    else if (strcmp(field, "endmcr") == 0)
    {
        macro[0] = '\0'; // Clear the macro buffer
        // Check for unexpected characters after "endmcr"
        if (!is_end_of_line(line[index]))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE; // Return false to indicate error
        }
    }
    else // Symbol is neither a macro nor a macro definition
    {
        // If currently defining a macro, insert the line into the macro table
        if (macro[0])
            insert(macroTable, macro, line);
        else
        {
            fputs(line, fp); // Otherwise, write the line to the output file
        }
    }
  
    return TRUE; // Return true to indicate successful processing
}


// int find_next(char *line, char *field)
// {
//     int index = 0;
//     int i = 0;

//     MOVE_TO_NOT_WHITE(line, index);

//     while (!is_end_of_line(line[index]) && !isspace(line[index]) && i < SYMBOL_MAX_SIZE)
//     {
//         field[i++] = line[index++];
//     }
//     field[i] = '\0';
//     if (is_end_of_line(line[index]) || isspace(line[index]))
//     {
//         return index;
//     }
//     field[0] = '\0';
//     return FALSE;
// }

/*
    char line[LINESIZE+2];
    char field[LINESIZE+2];
    unsigned char isMacro = 0;
    hashEntry *tmp;
    char *macro;
    unsigned char index = 0;

    while (fgets(line, LINESIZE+2, fp) != EOF)
    {
        MOVE_TO_NOT_WHITE(line, index);
        if (line[index] == 0 || line[index] == ';')
        {
            continue;
        }

        while(line[index] && !isspace(line[index]) && i < LINESIZE+2) {
            field[i++] = line[index++];
        }
        field[i] = '\0';

        if (!is_valid_macro(field)) {

        }
        tmp = lookup(field);
        if (tmp != NULL)
        {
            // add all lines associated with the macro
            for (node *current = tmp->lines; current != NULL; current = current->next)
            {

                fputs(current->line, file);
            }
        }
        else if (strcmp(token, "mcr") == 0)
        {
            isMacro = 1;
            token = strtok(NULL, " ");
            macro = strdup(token);

            // check and validate the macro name
            if ((macro == NULL) || (isalpha(*macro) == 0))
            {
                printf("Error: Memory allocation failed.");
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(token, "endmcr") == 0)
        {
            isMacro = 0;
            free(macro);
        }
        else if (isMacro)
        {
            insert(macro, line);
        }
        else
        {
            fputs(line, file);
        }
    }
    free(outputName);
    fclose(fp);
    fclose(file);
    return EXIT_SUCCESS;
}


*/
