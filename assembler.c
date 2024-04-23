#include <stdlib.h>
#include "firstPass.h"
#include "secondPass.h"
#include "preAssembler.h"
#include "vars.h"
#include "utils.h"
#include "writeFiles.h"

const char base4[4] = {'*', '#', '%', '!'};

unsigned int data[MAX_MEMORY_SIZE - RESERVED_MEMORY];
unsigned int instructions[MAX_MEMORY_SIZE - RESERVED_MEMORY];
Symbol *symbols = NULL;
external *externals = NULL;
hashTable *macroTable = NULL;

int main(int argc, char *argv[])
{
    int i;
    char *input_filename;
    FILE *file;
    FILE *fp;
    // Initialize macro table
    macroTable = initTable();

    // Loop through each command-line argument
    for (i = 1; i < argc; i++)
    {
        // Reset global variables for each input file
        reset_global_vars();

        // Create filename for input file with .as extension
        input_filename = create_file_name(argv[i], AS_FILE);

        // Open input file for reading
        file = fopen(input_filename, "r");

        // Check if the file was opened successfully
        if (file != NULL)
        {
            // Print pre-assembling process start message
            printf("************* Started %s pre_assembling process *************\n\n", input_filename);

            // Free memory allocated for input filename
            free(input_filename);

            // Create filename for output file with .am extension
            input_filename = create_file_name(argv[i], AM_FILE);

            // Open output file for writing
            fp = fopen(input_filename, "w");

            // Check if the output file was created successfully
            if (fp != NULL)
            {
                // Perform pre-assembly process
                preAssembler(file, fp);

                // Close output file
                fclose(fp);

                // Reopen output file for reading
                fp = fopen(input_filename, "r");

                // Check if there were no errors in pre-assembly process
                if (!has_error)
                {
                    // Print assembling process start message
                    printf("\n************* Started %s assembling process *************\n\n", input_filename);

                    // Perform first pass of assembly process
                    first_pass(fp);
                }

                // Check if there were no errors in first pass
                if (!has_error)
                {
                    // Rewind output file to beginning
                    rewind(fp);

                    // Perform second pass of assembly process
                    second_pass(fp);
                }

                // Check if there were no errors in second pass
                if (!has_error)
                {
                    // Write output files
                    write_output_files(argv[i]);

                    // Print assembling process finish message
                    printf("\n************* Finished %s assembling process *************\n\n", input_filename);
                }
                else
                {
                    // Print assembling process Failed message
                    printf("\n************* Failed %s assembling process *************\n\n", input_filename);
                }

                // Close output file
                fclose(fp);
                free(input_filename);
            }
            else
                print_error_message(FAILED_TO_CREATE_FILE, 0);
        }
        else
        {
            print_error_message(CANNOT_OPEN_FILE, 0);
        }
        reset_global_vars();

        return 0;
    }
}
