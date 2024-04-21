#include "utils.h"
#include "vars.h"
#include "writeFiles.h"
#include <stdlib.h>

void write_output_files(char *filename)
{
    FILE *file = open_file(filename, OB_FILE);
    write_output_ob(file);

    if (has_entry)
    {
        file = open_file(filename, ENT_FILE);
        write_output_entry(file);
    }
    if (has_external)
    {
        file = open_file(filename, EXT_FILE);
        write_output_external(file);
    }
}

FILE *open_file(char *filename, FILE_TYPE type)
{
    FILE *file;
    char *filename_str;
    filename_str = create_file_name(filename, type);
    file = fopen(filename_str, "w");
    free(filename_str);
    
    if (file == NULL)
    {
        err = CANNOT_OPEN_FILE;
        return NULL;
    }
    return file;
}

void write_output_ob(FILE *fp)
{
    unsigned int address = RESERVED_MEMORY;
    int i;
    char *param;

    fprintf(fp, "%d\t%d\n", ic, dc); /* First line */

    for (i = 0; i < ic; address++, i++) /* Instructions memory */
    {
        param = convert_to_base_4(instructions[i]);

        fprintf(fp, "%d\t%s\n", address, param);

        free(param);
    }
    for (i = 0; i < dc; address++, i++) /* Data memory */
    {
        param = convert_to_base_4(data[i]);

        fprintf(fp, "%d\t%s\n", address, param);

        free(param);
    }

    fclose(fp);
}

void write_output_entry(FILE *fp)
{

    Symbol *current = symbols;
    /* Go through symbols table and print only symbols that have an entry */
    while (current)
    {
        if (current->attribute == ENTRY)
        {
            fprintf(fp, "%s\t%d\n", current->name, current->value);
        }
        current = current->next;
    }
    fclose(fp);
}

void write_output_external(FILE *fp)
{
    external *current = externals;
    /* Go through external table and print them */
    while (current)
    {

        fprintf(fp, "%s\t%d\n", current->name, current->address);

        current = current->next;
    }
    fclose(fp);
}

char *create_file_name(char *filename, FILE_TYPE type)
{
    switch (type)
    {
    case AS_FILE:
        return strallocat(filename, ".as");
    case AM_FILE:
        return strallocat(filename, ".am");
    case OB_FILE:
        return strallocat(filename, ".ob");
    case ENT_FILE:
        return strallocat(filename, ".ent");
    case EXT_FILE:
        return strallocat(filename, ".ext");
    }
}