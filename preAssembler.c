#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "utils.h"
#include "preAssembler.h"
#include "vars.h"
#include "hashtable.h"

void preAssembler(FILE *file, FILE *fp)
{
    char line[LINESIZE + 2];
    char macro[SYMBOL_MAX_SIZE + 1];
    unsigned char index = 0;
    int line_num = 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        err = FALSE;
        warn = FALSE;
        if (strlen(line) == LINESIZE + 1 && line[LINESIZE] != '\n')
        { // Check if last character is not newline

            warn = WARNING_LINE_TOO_LONG;
            print_error_message(warn, line_num);

            // Flush the rest of the line to avoid processing remnants
            int ch;
            while ((ch = fgetc(file)) != '\n' && ch != EOF)
                ;
            line[LINESIZE-1] = '\n';
            line[LINESIZE+1] = '\0'; // Truncate line
        }
        if (!pre_process_line(line, fp, macro))
        {
            print_error_message(err, line_num);
        }

        line_num++;
    }
}

int pre_process_line(char *line, FILE *fp, char *macro)
{
    int index = 0;
    char field[SYMBOL_MAX_SIZE + 1];

    MOVE_TO_NOT_WHITE(line, index);

    if (is_end_of_line(line[index]) || line[index] == ';')
        return TRUE;

    int length = find_next_symbol(&line[index], field, ' ');
    index += length;

    if (length == SYMBOL_MAX_SIZE)
        field[0] = '\0';

    node *tmp = lookup(macroTable, field);
    MOVE_TO_NOT_WHITE(line, index);

    if (tmp != NULL)
    {
        if (!is_end_of_line(line[index]))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE;
        }
        for (node *current = tmp; current != NULL; current = current->next)
            fputs(current->line, fp);
        return TRUE;
    }

    if (strcmp(field, "mcr") == 0)
    {
        length = find_next_symbol(&line[index], macro, ' ');
        index += length;
        if (length == SYMBOL_MAX_SIZE)
            field[0] = '\0';

        MOVE_TO_NOT_WHITE(line, index);

        if (!is_end_of_line(line[index]) || (macro[0] && !is_valid_macro(macro)))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE;
        }
    }
    else if (strcmp(field, "endmcr") == 0)
    {
        macro[0] = '\0';
        if (!is_end_of_line(line[index]))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE;
        }
    }
    else
    {
        if (macro[0])
            insert(macroTable, macro, line);
        else
        {
            fputs(line, fp);
        }
    }
  
    return TRUE;
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
