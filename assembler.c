#include <stdlib.h>
#include "first_pass.h"
#include "second_pass.h"
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
    macroTable = initTable();

    for (i = 1; i < argc; i++)
    {

        reset_global_vars();

        input_filename = create_file_name(argv[i], AS_FILE); /* Appending .as to filename */
        file = fopen(input_filename, "r");
        if (fp != NULL)
        {
            printf("************* Started %s pre_assembling process *************\n\n", input_filename);
            free(input_filename);
            input_filename = create_file_name(argv[i], AM_FILE); /* Appending .am to filename */
            fp = fopen(input_filename, "w");
            preAssembler(file, fp);
            fclose(fp);
            fp = fopen(input_filename, "r");

            printf("************* FINISHED %s pre_assembling process *************\n\n", input_filename);

            if (!has_error)
            {
                printf("************* Started %s assembling process *************\n\n", input_filename);

                first_pass(fp);
            }

            if (!has_error)
            { /* proceed to second pass */
                rewind(fp);

                second_pass(fp);
            }
            if (!has_error)
            {
                write_output_files(argv[i]);
                printf("************* FINISHED %s assembling process *************\n\n", input_filename);
            }

            fclose(fp);
        }
        else
            free(input_filename);
    }
    reset_global_vars();

    return 0;
}
