#include <stdlib.h>
#include "globals.h"

void second_pass(FILE *fp);
int process_line_second_pass(char *line);
int process_operation(opcode operation, char *args);
int encode_additional_words(char *src_operand, char *dst_operand, addressing_type src_type, addressing_type dst_type);
unsigned int build_register_word(int is_dst, char *reg);
int encode_label(char *symbol);
int encode_additional_word(int is_dst, addressing_type type, char *operand);
int handle_immediate_address(char *operand, unsigned int *word);
int handle_index_address(char *operand, unsigned int *word);
