#include "utils.h"

void first_pass(FILE *fp);
unsigned int build_first_word(opcode operation, addressing_type first_operand, addressing_type second_operand);
int num_words(addressing_type operand);
int process_line(char *line);
int process_code(char *line);
int calculate_command_num_additional_words(addressing_type first_operand, addressing_type second_operand);
int command_accept_methods(opcode type, addressing_type first_operand, addressing_type second_operand);
