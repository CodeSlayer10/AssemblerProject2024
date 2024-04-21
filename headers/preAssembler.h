#include <stdio.h>

void preAssembler(FILE *file, FILE *fp);

int pre_process_line(char *line, FILE *fp, char *macro);

int find_next(char *line, char *field);

unsigned int build_register_word(int is_dst, char *reg);