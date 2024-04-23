#include "utils.h"
#include "vars.h"
#include "writeFiles.h"
#include <stdlib.h>

/**
 * Writes output files based on the given filename.
 * @param filename The name of the output files.
 */
void write_output_files(char *filename)
{
    FILE *file = open_file(filename, OB_FILE); // Open object file
    write_output_ob(file);                     // Write object file contents

    // If there are entry symbols, write entry file
    if (has_entry)
    {
        file = open_file(filename, ENT_FILE);
        write_output_entry(file);
    }
    // If there are external symbols, write external file
    if (has_external)
    {
        file = open_file(filename, EXT_FILE);
        write_output_external(file);
    }
}

/**
 * Opens a file with the given filename and file type.
 * @param filename The name of the file.
 * @param type The type of the file (OB_FILE, ENT_FILE, or EXT_FILE).
 * @return Returns a pointer to the opened file.
 */
FILE *open_file(char *filename, FILE_TYPE type)
{
    FILE *file;                                      // File pointer
    char *filename_str;                              // String for the filename with appropriate extension
    filename_str = create_file_name(filename, type); // Create the filename string with appropriate extension
    file = fopen(filename_str, "w");                 // Open the file in write mode
    free(filename_str);                              // Free the dynamically allocated filename string

    if (file == NULL)
    {
        err = CANNOT_OPEN_FILE; // Set error if file cannot be opened
        return NULL;            // Return NULL if file cannot be opened
    }
    return file; // Return the file pointer
}

/**
 * Writes all data and instructions to the object file.
 * @param fp Pointer to the object file.
 */
void write_output_ob(FILE *fp)
{
    unsigned int address = RESERVED_MEMORY; // Address counter
    int i;                                  // Loop variable
    char *param;                            // Temporary parameter string

    // Write the number of instruction and data lines in the first line of the object file.
    fprintf(fp, "%d\t%d\n", ic, dc);

    // Write instructions memory to the object file.
    for (i = 0; i < ic; address++, i++)
    {
        param = convert_to_base_4(instructions[i]); // Convert instruction to base-4
        fprintf(fp, "%d\t%s\n", address, param);    // Write address and corresponding instruction
        free(param);                                // Free the allocated memory for the parameter
    }

    // Write data memory to the object file.
    for (i = 0; i < dc; address++, i++)
    {
        param = convert_to_base_4(data[i]);      // Convert data to base-4
        fprintf(fp, "%d\t%s\n", address, param); // Write address and corresponding data
        free(param);                             // Free the allocated memory for the parameter
    }

    fclose(fp); // Close the file
}

/**
 * Writes entry symbols to the external file.
 * @param fp Pointer to the entry file.
 */
void write_output_entry(FILE *fp)
{
    Symbol *current = symbols; // Pointer to traverse the symbol table

    // Iterate through symbol table and print symbols marked as entry.
    while (current)
    {
        if (current->attribute == ENTRY)
        {
            fprintf(fp, "%s\t%d\n", current->name, current->value); // Write entry symbol name and value
        }
        current = current->next; // Move to the next symbol in the symbol table
    }
    fclose(fp); // Close the file
}

/**
 * Writes external symbols to the external file.
 * @param fp Pointer to the external file.
 */
void write_output_external(FILE *fp)
{
    external *current = externals; // Pointer to traverse the external symbol table

    // Iterate through external symbol table and print symbols.
    while (current)
    {
        fprintf(fp, "%s\t%d\n", current->name, current->address); // Write external symbol name and address
        current = current->next;                                  // Move to the next external symbol
    }
    fclose(fp); // Close the file
}

/**
 * Creates a filename with the appropriate extension based on the file type.
 * @param filename The base filename.
 * @param type The type of the file (AS_FILE, AM_FILE, OB_FILE, ENT_FILE, or EXT_FILE).
 * @return Returns the filename with the appropriate extension.
 */
char *create_file_name(char *filename, FILE_TYPE type)
{
    switch (type)
    {
    case AS_FILE:
        return strallocat(filename, ".as"); // Append ".as" extension for assembly source file
    case AM_FILE:
        return strallocat(filename, ".am"); // Append ".am" extension for assembled machine code file
    case OB_FILE:
        return strallocat(filename, ".ob"); // Append ".ob" extension for object file
    case ENT_FILE:
        return strallocat(filename, ".ent"); // Append ".ent" extension for entry file
    case EXT_FILE:
        return strallocat(filename, ".ext"); // Append ".ext" extension for external file
    }
}
