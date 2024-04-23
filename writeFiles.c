#include "utils.h"
#include "vars.h"
#include "writeFiles.h"
#include <stdlib.h>

// Writes output files based on the given filename.
void write_output_files(char *filename)
{
    FILE *file = open_file(filename, OB_FILE); // Open object file.
    write_output_ob(file); // Write object file contents.

    // If there are entry symbols, write entry file.
    if (has_entry)
    {
        file = open_file(filename, ENT_FILE);
        write_output_entry(file);
    }
    // If there are external symbols, write external file.
    if (has_external)
    {
        file = open_file(filename, EXT_FILE);
        write_output_external(file);
    }
}

// Opens a file with the given filename and type.
FILE *open_file(char *filename, FILE_TYPE type)
{
    FILE *file;
    char *filename_str;
    filename_str = create_file_name(filename, type); // Create the filename string with appropriate extension.
    file = fopen(filename_str, "w"); // Open the file in write mode.
    free(filename_str); // Free the dynamically allocated filename string.
    
    if (file == NULL)
    {
        err = CANNOT_OPEN_FILE; // Set error if file cannot be opened.
        return NULL;
    }
    return file;
}

// Writes output to the object file.
void write_output_ob(FILE *fp)
{
    unsigned int address = RESERVED_MEMORY;
    int i;
    char *param;

    // Write the number of instruction and data lines in the first line of the object file.
    fprintf(fp, "%d\t%d\n", ic, dc);

    // Write instructions memory to the object file.
    for (i = 0; i < ic; address++, i++)
    {
        param = convert_to_base_4(instructions[i]);
        fprintf(fp, "%d\t%s\n", address, param); // Write address and corresponding instruction.
        free(param);
    }

    // Write data memory to the object file.
    for (i = 0; i < dc; address++, i++)
    {
        param = convert_to_base_4(data[i]);
        fprintf(fp, "%d\t%s\n", address, param); // Write address and corresponding data.
        free(param);
    }

    fclose(fp); // Close the file.
}

// Writes entry symbols to the entry file.
void write_output_entry(FILE *fp)
{
    Symbol *current = symbols;

    // Iterate through symbol table and print symbols marked as entry.
    while (current)
    {
        if (current->attribute == ENTRY)
        {
            fprintf(fp, "%s\t%d\n", current->name, current->value); // Write entry symbol name and value.
        }
        current = current->next;
    }
    fclose(fp); // Close the file.
}

// Writes external symbols to the external file.
void write_output_external(FILE *fp)
{
    external *current = externals;

    // Iterate through external symbol table and print symbols.
    while (current)
    {
        fprintf(fp, "%s\t%d\n", current->name, current->address); // Write external symbol name and address.
        current = current->next;
    }
    fclose(fp); // Close the file.
}

// Creates a filename with the appropriate extension based on the file type.
char *create_file_name(char *filename, FILE_TYPE type)
{
    switch (type)
    {
    case AS_FILE:
        return strallocat(filename, ".as"); // Append ".as" extension for assembly source file.
    case AM_FILE:
        return strallocat(filename, ".am"); // Append ".am" extension for assembled machine code file.
    case OB_FILE:
        return strallocat(filename, ".ob"); // Append ".ob" extension for object file.
    case ENT_FILE:
        return strallocat(filename, ".ent"); // Append ".ent" extension for entry file.
    case EXT_FILE:
        return strallocat(filename, ".ext"); // Append ".ext" extension for external file.
    }
}
