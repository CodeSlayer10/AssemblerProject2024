#include <stdio.h>
#include "globals.h"

#define MOVE_TO_NOT_WHITE(string, index)                \
    while (string[(index)] && isspace(string[(index)])) \
        ++(index);

#define CONCAT(a, b) a##b

void *checkedAlloc(long size);
char *strallocat(char *s0, char *s1);
int is_reserved(char *name, int is_symbol);
int is_valid_symbol(char *symbol);
int is_valid_macro(char *macro);
instruction find_instruction_by_name(char *name);
reg find_register_by_name(char *name);
opcode find_operation_by_name(char *name);
int is_int_str(char *str);
int is_alphanum_str(char *str);
int is_printable_str(char *str);
int is_in_range(int num);
int is_end_of_line(char chr);
int validate_operand_count_by_opcode(opcode operation, int count);
int get_operand_count_by_opcode(opcode operation);
int insert_data(int num);
int insert_instructions(int num);
unsigned int insert_are(unsigned int info, ARE are);
unsigned int extract_bits(unsigned int word, int start, int end);
char *convert_to_base_4(unsigned int num);
void reset_global_vars();
int find_next_symbol(char *line, char *symbol, char del);
int find_next_token(char *line, char *token, char del);
void print_error_message(error error_code, int line_num);