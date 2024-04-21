#include <stdio.h>
#include "globals.h"

void write_output_ob(FILE *fp);

void write_output_entry(FILE *fp);

void write_output_external(FILE *fp);

char *create_file_name(char *filename, FILE_TYPE type);

void write_output_files(char *filename);

FILE *open_file(char *filename, FILE_TYPE type);