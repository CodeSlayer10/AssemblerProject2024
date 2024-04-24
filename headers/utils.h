#include <stdio.h>
#include "globals.h"

#define MOVE_TO_NOT_WHITE(string, index)                \
    while (string[(index)] && isspace(string[(index)])) \
        ++(index); // Macro to move index past white spaces in the string

#define CONCAT(a, b) a##b // Macro to concatenate two identifiers

void *checkedAlloc(long size);                                     // Allocates memory with a NULL check.
char *strallocat(char *s0, char *s1);                              // Allocates memory and concatenates two strings.
int is_reserved(char *name, int is_symbol);                        // Checks if a given name is reserved.
int is_valid_symbol(char *symbol);                                 // Checks if a symbol is valid.
int is_valid_macro(char *macro);                                   // Checks if a macro name is valid.
instruction find_instruction_by_name(char *name);                  // Finds an instruction by its name in the instruction lookup table.
reg find_register_by_name(char *name);                             // Finds a register by its name in the register lookup table.
opcode find_operation_by_name(char *name);                         // Finds an operation by its name in the operation lookup table.
int is_int_str(char *str);                                         // Checks if a string represents an integer.
int is_alphanum_str(char *str);                                    // Checks if a string contains only alphanumeric characters.
int is_printable_str(char *str);                                   // Checks if a string contains only printable characters.
int is_in_range(int num);                                          // Checks if a number is within the range of a signed 12-bit integer.
int is_end_of_line(char chr);                                      // Checks if a character is the end of a line ('\0' or '\n').
int validate_operand_count_by_opcode(opcode operation, int count); // Validates the operand count for an operation based on its opcode.
int get_operand_count_by_opcode(opcode operation);                 // Retrieves the operand count for an operation based on its opcode.
int insert_data(int num);                                          // Inserts data into the data array.
int insert_instructions(int num);                                  // Inserts instructions into the instructions array.
unsigned int insert_are(unsigned int info, ARE are);               // Inserts the Addressing-Relocation-External (ARE) bits into the given word.
unsigned int extract_bits(unsigned int word, int start, int end);  // Extracts a sequence of bits from a word, given start and end positions of the bit-sequence (0 is LSB).
char *convert_to_base_4(unsigned int num);                         // Converts a number to its encoded base-4 representation.
void reset_global_vars();                                          // Resets global variables.
int find_next_symbol(char *line, char *symbol, char del);          // Finds the next symbol in a line.
int find_next_token(char *line, char *token, char del);            // Finds the next token in a line.
void print_error_message(error error_code, int line_num);          // Prints the corrsponding error message for a given error code.
