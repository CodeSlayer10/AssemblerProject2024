#include "first_pass.h"
#include "second_pass.h"
#include "vars.h"
#include "utils.h"
#include "writeFiles.h"
#include <stdlib.h>

// #include "preAssembler.h"

const char base4[4] = {'*', '#', '%', '!'};

unsigned int data[MAX_MEMORY_SIZE - RESERVED_MEMORY];
unsigned int instructions[MAX_MEMORY_SIZE - RESERVED_MEMORY];
Symbol *symbols = NULL;
external *externals = NULL;

int main(int argc, char *argv[])
{
    int i;
    char *input_filename;
    FILE *file;
    FILE *fp;
    // hashEntry **macroTable = (hashEntry **)malloc(HASHSIZE * sizeof(hashEntry *));
    // if (macroTable == NULL)
    // {
    //     return 1;
    // }
    // for (int i = 0; i < HASHSIZE; i++)
    // {
    //     macroTable[i] = NULL;
    // }

    for (i = 1; i < argc; i++)
    {
        input_filename = create_file_name(argv[i], AS_FILE); /* Appending .as to filename */
        fp = fopen(input_filename, "r");
        if (fp != NULL)
        {
            // printf("************* Started %s pre_assembling process *************\n\n", input_filename);
            // free(input_filename);
            // input_filename = create_file_name(argv[i], AM_FILE); /* Appending .am to filename */
            // fp = fopen(input_filename, "w");
            // preAssembler(file, fp);
            // if (err)
            // {
            //     return 1;
            // }
            // printf("************* FINISHED %s pre_assembling process *************\n\n", input_filename);

            printf("************* Started %s assembling process *************\n\n", input_filename);

            reset_global_vars();

            first_pass(fp);
            if (warn)
            {
                printf("warning: %d\n", warn);
            }

            if (!has_error)
            { /* proceed to second pass */
                rewind(fp);

                second_pass(fp);
            }
            if (!has_error)
            {
                write_output_files(argv[i]);
            }
            reset_global_vars();
            printf("\n\n************* Finished %s assembling process *************\n\n", input_filename);
        }
        else
            free(input_filename);
    }

    return 0;
}