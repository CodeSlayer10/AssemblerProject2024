#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "hashtable.h"
#include "utils.h"
#include "vars.h"

void preAssembler(FILE *file, FILE *fp)
{

    char line[LINESIZE + 2];
    char macro[SYMBOL_MAX_SIZE + 1];
    unsigned char index = 0;
    int line_num = 1;
    err = FALSE;

    while (fgets(line, LINESIZE + 2, file) != EOF)
    {

        MOVE_TO_NOT_WHITE(line, index);
        if (line[index] == 0 || line[index] == ';')
        {
            continue;
        }
        if (strlen(line) == LINESIZE + 1 && line[LINESIZE] != '\n')
        {
            warn = WARNING_LINE_TOO_LONG;
        }
        if (!pre_process_line(line, fp, macro))
        {
            write_error(line_num);
        }
        line_num++;
    }
}

int pre_process_line(char *line, FILE *fp, char *macro)
{
    int index = 0;
    int length = 0;
    char field[SYMBOL_MAX_SIZE + 1];

    hashEntry *tmp;

    length += find_next_symbol(&line[index], field, ' ');
    index += length;
    if (length == SYMBOL_MAX_SIZE)
    {
        field[0] = '\0';
    }
    length = 0;
    tmp = lookup(macroTable, field);
    MOVE_TO_NOT_WHITE(line, index);
    if (tmp != NULL)
    {
        if (!is_end_of_line(line[index]))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE;
        }
        for (node *current = tmp->lines; current != NULL; current = current->next)
        {
            fputs(current->line, fp);
        }
        return TRUE;
    }
    if (strcmp(field, "mcr"))
    {
        length += find_next_symbol(&line[index], macro, ' ');
        index += length;

        if (length == SYMBOL_MAX_SIZE)
        {
            field[0] = '\0';
        }

        MOVE_TO_NOT_WHITE(line, index);
        if (!is_end_of_line(line[index]))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE;
        }
        if (macro[0] && !is_valid_macro(macro))
        {
            macro[0] = '\0';
            return FALSE;
        }
    }
    else if (strcmp(field, "endmcr"))
    {
        macro[0] = '\0';
        if (!is_end_of_line(line[index]))
        {
            err = MACRO_UNEXPECTED_CHARS;
            return FALSE;
        }
    }
    else if (macro[0])
    {
        insert(macroTable, macro, line);
    }
    else
    {
        fputs(line, fp);
    }
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
