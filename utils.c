#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "utils.h"
#include "vars.h"

/**
 * Lookup table for opcode operations.
 */
struct operationLookupItem
{
	char *name;		  // Operation name
	opcode operation; // Corresponding opcode
	int operands;	  // Number of operands
} operationLookupTable[] = {
	{"mov", MOV_OP, 2}, // Move operation
	{"cmp", CMP_OP, 2}, // Compare operation
	{"add", ADD_OP, 2}, // Add operation
	{"sub", SUB_OP, 2}, // Subtract operation
	{"not", NOT_OP, 1}, // Not operation
	{"clr", CLR_OP, 1}, // Clear operation
	{"lea", LEA_OP, 2}, // Load effective address operation
	{"inc", INC_OP, 1}, // Increment operation
	{"dec", DEC_OP, 1}, // Decrement operation
	{"jmp", JMP_OP, 1}, // Jump operation
	{"bne", BNE_OP, 1}, // Branch if not equal operation
	{"red", RED_OP, 1}, // Read operation
	{"prn", PRN_OP, 1}, // Print operation
	{"jsr", JSR_OP, 1}, // Jump to subroutine operation
	{"rts", RTS_OP, 0}, // Return from subroutine operation
	{"hlt", HLT_OP, 0}, // Halt operation
	{NULL, NONE_OP, 0}, // End of table marker
};

/**
 * Lookup table for assembler instructions.
 */
struct instructionLookupItem
{
	char *name;				 // Instruction name
	instruction instruction; // Corresponding instruction code
	attribute attribute;	 // Instruction attribute
} instructionLookupTable[] = {
	{".define", DEFINE_IN, MDEFINE},  // Define directive
	{".string", STRING_IN, DATA},	  // String directive
	{".data", DATA_IN, DATA},		  // Data directive
	{".entry", ENTRY_IN, ENTRY},	  // Entry directive
	{".extern", EXTERN_IN, EXTERNAL}, // Extern directive
	{NULL, NONE_IN, NONE},			  // End of table marker
};

/**
 * Lookup table for registers.
 */
struct registerLookupItem
{
	char *name; // Register name
	reg reg;	// Corresponding register code
} registerLookupTable[] = {
	{"r0", R0},		// Register R0
	{"r1", R1},		// Register R1
	{"r2", R2},		// Register R2
	{"r3", R3},		// Register R3
	{"r4", R4},		// Register R4
	{"r5", R5},		// Register R5
	{"r6", R6},		// Register R6
	{"r7", R7},		// Register R7
	{NULL, R_NONE}, // End of table marker
};

/**
 * Allocates memory with a NULL check.
 * @param size The size of the memory to allocate.
 * @return Returns a pointer to the allocated memory.
 */
void *checkedAlloc(long size)
{
	void *ptr = malloc(size); // Allocate memory
	if (ptr == NULL)
	{
		reset_global_vars();										 // Reset global variables on memory allocation failure
		fprintf(stderr, "Fatal error: Memory allocation failed.\n"); // Print error message
	}
	return ptr; // Return the allocated memory pointer
}

/**
 * Allocates memory and concatenates two strings.
 * @param s0 The first string.
 * @param s1 The second string.
 * @return Returns a pointer to the concatenated string.
 */
char *strallocat(char *s0, char *s1)
{
	// Allocate memory for the concatenated string
	char *str = (char *)checkedAlloc(strlen(s0) + strlen(s1) + 1);
	// Copy the first string into the allocated memory
	strcpy(str, s0);
	// Concatenate the second string onto the end of the first string
	strcat(str, s1);
	return str; // Return the concatenated string
}

/**
 * Checks if a given name is reserved.
 * @param name The name to check.
 * @param is_label Flag indicating if the name is intended to be a label.
 * @return Returns TRUE if the name is reserved, FALSE otherwise.
 */
int is_reserved(char *name, int is_label)
{
	// Check if the name is an instruction
	if (find_instruction_by_name(name) != NONE_IN)
	{
		// Set appropriate error message based on whether the name is intended to be a label or a macro
		err = is_label ? LABEL_CANT_BE_INSTRUCT : MACRO_CANT_BE_INSTRUCT;
		return TRUE; // Return TRUE to indicate the name is reserved
	}

	// Check if the name is a register
	if (find_register_by_name(name) != R_NONE)
	{
		// Set appropriate error message based on whether the name is intended to be a label or a macro
		err = is_label ? LABEL_CANT_BE_REGISTER : MACRO_CANT_BE_REGISTER;
		return TRUE; // Return TRUE to indicate the name is reserved
	}

	// Check if the name is an operation command
	if (find_operation_by_name(name) != NONE_OP)
	{
		// Set appropriate error message based on whether the name is intended to be a label or a macro
		err = is_label ? LABEL_CANT_BE_COMMAND : MACRO_CANT_BE_COMMAND;
		return TRUE; // Return TRUE to indicate the name is reserved
	}

	return FALSE; // Return FALSE to indicate the name is not reserved
}

/**
 * Checks if a symbol is valid.
 * @param symbol The symbol to check.
 * @return Returns TRUE if the symbol is valid, FALSE otherwise.
 */
int is_valid_symbol(char *symbol)
{
	// Check if the symbol is an empty string
	if (!symbol[0])
	{
		err = WARNING_EMPTY_LABEL; // Set warning message for empty label
		return FALSE;			   // Return FALSE to indicate invalid symbol
	}

	// Check if the first character of the symbol is alphabetic
	if (!isalpha(symbol[0]))
	{
		err = LABEL_INVALID_FIRST_CHAR; // Set error message for invalid first character
		return FALSE;					// Return FALSE to indicate invalid symbol
	}

	// Check if the length of the symbol exceeds the maximum allowed size
	if (strlen(symbol) > SYMBOL_MAX_SIZE)
	{
		err = LABEL_TOO_LONG; // Set error message for symbol being too long
		return FALSE;		  // Return FALSE to indicate invalid symbol
	}

	// Check if the symbol is reserved
	if (is_reserved(symbol, TRUE))
	{
		return FALSE; // Return FALSE to indicate invalid symbol
	}

	// Check if the symbol contains only alphanumeric characters
	if (!is_alphanum_str(symbol))
	{
		err = LABEL_ONLY_ALPHANUMERIC; // Set error message for non-alphanumeric characters
		return FALSE;				   // Return FALSE to indicate invalid symbol
	}

	// Check if the symbol is already defined as a macro
	if (lookup(macroTable, symbol) != NULL)
	{
		err = LABEL_CANT_BE_MACRO; // Set error message for symbol being a macro
		return FALSE;			   // Return FALSE to indicate invalid symbol
	}

	// If all conditions are met, return true
	return TRUE; // Return TRUE to indicate valid symbol
}

/**
 * Checks if a macro name is valid.
 * @param macro The macro name to check.
 * @return Returns TRUE if the macro name is valid, FALSE otherwise.
 */
int is_valid_macro(char *macro)
{
	// Check if the macro name is an empty string
	if (!macro[0])
	{
		err = MACRO_CANT_BE_EMPTY; // Set error message for empty macro name
		return FALSE;			   // Return FALSE to indicate invalid macro name
	}

	// Check if the first character of the macro name is alphabetic
	if (!isalpha(macro[0]))
	{
		err = MACRO_INVALID_FIRST_CHAR; // Set error message for invalid first character
		return FALSE;					// Return FALSE to indicate invalid macro name
	}

	// Check if all characters in the macro name are printable
	if (!is_printable_str(macro))
	{
		err = MACRO_ONLY_PRINTABLE; // Set error message for non-printable characters
		return FALSE;				// Return FALSE to indicate invalid macro name
	}

	// Check if the macro name is reserved
	if (is_reserved(macro, FALSE))
	{
		return FALSE; // Return FALSE to indicate invalid macro name
	}

	// If all conditions are met, return true
	return TRUE; // Return TRUE to indicate valid macro name
}

/**
 * Finds an instruction by its name in the instruction lookup table.
 * @param name The name of the instruction to find.
 * @return Returns the instruction code if found, NONE_IN otherwise.
 */
instruction find_instruction_by_name(char *name)
{
	int i = 0;	   // Index variable
	char *current; // Pointer to the current instruction name
	while ((current = instructionLookupTable[i].name) != NULL)
	{
		// Compare current instruction name with the given name
		if (strcmp(current, name) == 0)
			return instructionLookupTable[i].instruction; // Return instruction if found
		i++;											  // Move to the next entry in the lookup table
	}
	return NONE_IN; // Return NONE_IN if instruction not found
}

/**
 * Finds a register by its name in the register lookup table.
 * @param name The name of the register to find.
 * @return Returns the register code if found, R_NONE otherwise.
 */
reg find_register_by_name(char *name)
{
	int i = 0;	   // Index variable
	char *current; // Pointer to the current register name
	while ((current = registerLookupTable[i].name) != NULL)
	{
		// Compare current register name with the given name
		if (strcmp(current, name) == 0)
			return registerLookupTable[i].reg; // Return register if found
		i++;								   // Move to the next entry in the lookup table
	}
	return R_NONE; // Return R_NONE if register not found
}

/**
 * Finds an operation by its name in the operation lookup table.
 * @param name The name of the operation to find.
 * @return Returns the opcode if found, NONE_OP otherwise.
 */
opcode find_operation_by_name(char *name)
{
	int i = 0;	   // Index variable
	char *current; // Pointer to the current operation name
	while ((current = operationLookupTable[i].name) != NULL)
	{
		// Compare current operation name with the given name
		if (strcmp(current, name) == 0)
			return operationLookupTable[i].operation; // Return operation if found
		i++;										  // Move to the next entry in the lookup table
	}
	return NONE_OP; // Return NONE_OP if operation not found
}

/**
 * Checks if a string represents an integer.
 * @param str The string to check.
 * @return Returns TRUE if the string represents an integer, FALSE otherwise.
 */
int is_int_str(char *str)
{
	int i = 0; // Index variable

	// Skip leading sign character, if any
	if (str[0] == '+' || str[0] == '-')
		str++;

	// Iterate through the string to check if all characters are digits
	for (; str[i] && isdigit(str[i]); i++)
		;

	// Check if the string is empty or if there are non-digit characters
	if (i == 0 || str[i] != '\0')
	{
		return FALSE; // Return FALSE to indicate not an integer string
	}
	return TRUE; // Return TRUE to indicate an integer string
}

/**
 * Checks if a string contains only alphanumeric characters.
 * @param str The string to check.
 * @return Returns TRUE if the string contains only alphanumeric characters, FALSE otherwise.
 */
int is_alphanum_str(char *str)
{
	int i = 0; // Index variable

	// Iterate through the string to check if all characters are alphanumeric
	for (; str[i] && isalnum(str[i]); i++)
		;

	// Check if the string is empty or if there are non-alphanumeric characters
	if (i == 0 || str[i] != '\0')
	{
		return FALSE; // Return FALSE to indicate not an alphanumeric string
	}
	return TRUE; // Return TRUE to indicate an alphanumeric string
}

/**
 * Checks if a string contains only printable characters.
 * @param str The string to check.
 * @return Returns TRUE if the string contains only printable characters, FALSE otherwise.
 */
int is_printable_str(char *str)
{
	int i = 0; // Index variable

	// Iterate through the string to check if all characters are printable
	for (; str[i] && isprint(str[i]); i++)
		;

	// Check if the string is empty or if there are non-printable characters
	if (i == 0 || str[i] != '\0')
	{
		return FALSE; // Return FALSE to indicate not a printable string
	}
	return TRUE; // Return TRUE to indicate a printable string
}

/**
 * Checks if a number is within the range of a signed 12-bit integer.
 * @param num The number to check.
 * @return Returns TRUE if the number is within the range, FALSE otherwise.
 */
int is_in_range(int num)
{
	// Calculate the minimum and maximum values of a signed 12-bit integer
	int min_val = -(1 << 11);
	int max_val = (1 << 11) - 1;

	// Check if the number is within the range
	return (num >= min_val && num <= max_val);
}

/**
 * Checks if a character is the end of a line ('\0' or '\n').
 * @param chr The character to check.
 * @return Returns TRUE if the character is the end of a line, FALSE otherwise.
 */
int is_end_of_line(char chr)
{
	return chr == '\0' || chr == '\n';
}

/**
 * Validates the operand count for an operation based on its opcode.
 * @param operation The opcode of the operation.
 * @param count The number of operands to validate.
 * @return Returns TRUE if the operand count matches the expected count for the operation, FALSE otherwise.
 */
int validate_operand_count_by_opcode(opcode operation, int count)
{
	return count == operationLookupTable[operation].operands; // Compare operand count with expected count
}

/**
 * Retrieves the operand count for an operation based on its opcode.
 * @param operation The opcode of the operation.
 * @return Returns the number of operands expected for the operation.
 */
int get_operand_count_by_opcode(opcode operation)
{
	return operationLookupTable[operation].operands; // Return the number of operands for the operation
}

/**
 * Inserts data into the data array.
 * @param num The data to insert.
 * @return Returns TRUE if the data was successfully inserted, FALSE otherwise.
 */
int insert_data(int num)
{
	// Check if there is enough memory to insert the data
	if (ic + dc + RESERVED_MEMORY > MAX_MEMORY_SIZE)
	{
		err = FAILED_TO_ALLOCATE_MEMORY; // Set error message for memory allocation failure
		return FALSE;					 // Return FALSE to indicate failure
	}
	data[dc++] = (unsigned int)num; // Insert data into the data array
	return TRUE;					// Return TRUE to indicate success
}

/**
 * Inserts instructions into the instructions array.
 * @param num The instruction to insert.
 * @return Returns TRUE if the instruction was successfully inserted, FALSE otherwise.
 */
int insert_instructions(int num)
{
	// Check if there is enough memory to insert the instruction
	if (ic + dc + RESERVED_MEMORY > MAX_MEMORY_SIZE)
	{
		err = FAILED_TO_ALLOCATE_MEMORY; // Set error message for memory allocation failure
		return FALSE;					 // Return FALSE to indicate failure
	}
	instructions[ic++] = (unsigned int)num; // Insert instruction into the instructions array
	return TRUE;							// Return TRUE to indicate success
}

/**
 * Inserts the Addressing-Relocation-External (ARE) bits into the given word.
 * @param info The word to insert ARE bits into.
 * @param are The ARE bits to insert.
 * @return Returns the information with the ARE bits inserted.
 */
unsigned int insert_are(unsigned int info, ARE are)
{
	return (info << BITS_IN_ARE) | are; // OR operand allows insertion of the 2 bits because 1 + 0 = 1
}

/**
 * Extracts a sequence of bits from a word, given start and end positions of the bit-sequence (0 is LSB).
 * @param word The word to extract bits from.
 * @param start The starting position of the bit-sequence.
 * @param end The ending position of the bit-sequence.
 * @return Returns the extracted bit-sequence.
 */
unsigned int extract_bits(unsigned int word, int start, int end)
{
	unsigned int result;				   // Variable to store the extracted bit-sequence
	int length = end - start + 1;		   // Length of bit-sequence
	unsigned int mask = (1 << length) - 1; // Creating a '111...1' mask with above line's length

	mask <<= start;		  // Moving mask to place of extraction
	result = word & mask; // The bits are now in their original position, and the rest is 0's
	result >>= start;	  // Moving the sequence to LSB
	return result;		  // Return the extracted bit-sequence
}

/**
 * Converts a number to its encoded base-4 representation.
 * @param num The number to convert.
 * @return Returns the base-4 representation of the number.
 */
char *convert_to_base_4(unsigned int num)
{
	char *base4_tmp = (char *)checkedAlloc(BASE4_SIZE);

	/* To convert from binary to base 4 we can just split it into chunks of 7 two bit characters */
	base4_tmp[0] = base4[extract_bits(num, 12, 13)];
	base4_tmp[1] = base4[extract_bits(num, 10, 11)];
	base4_tmp[2] = base4[extract_bits(num, 8, 9)];
	base4_tmp[3] = base4[extract_bits(num, 6, 7)];
	base4_tmp[4] = base4[extract_bits(num, 4, 5)];
	base4_tmp[5] = base4[extract_bits(num, 2, 3)];
	base4_tmp[6] = base4[extract_bits(num, 0, 1)];
	base4_tmp[7] = '\0';

	return base4_tmp; // Return the base-4 representation
}

/**
 * Resets global variables.
 */
void reset_global_vars()
{
	resetTable(macroTable);		// Reset macro table
	resetSymbolTable(&symbols); // Reset symbol table
	reset_ext(&externals);		// Reset external list
	has_entry = FALSE;			// Reset entry flag
	has_external = FALSE;		// Reset external flag
	has_error = FALSE;			// Reset error flag
	err = FALSE;				// Reset error code
	warn = FALSE;				// Reset warning flag
}

/**
 * Finds the next symbol in a line.
 * @param line The line to search for symbols.
 * @param symbol The buffer to store the found symbol.
 * @param del The delimiter character.
 * @return Returns the index of the next character after the symbol.
 */
int find_next_symbol(char *line, char *symbol, char del)
{
	int index = 0; // Index variable for traversing the line
	int i = 0;	   // Index variable for storing characters in the symbol buffer

	MOVE_TO_NOT_WHITE(line, index); // Move to the next non-white space character

	// Iterate until reaching the end of the line, a white space, the delimiter, or the maximum symbol size
	while (!is_end_of_line(line[index]) && !isspace(line[index]) && line[index] != del && i < SYMBOL_MAX_SIZE)
	{
		symbol[i++] = line[index++]; // Copy character to the symbol buffer and move to the next character
	}
	symbol[i] = '\0'; // Null-terminate the symbol buffer

	return index; // Return the index of the next character after the symbol
}

/**
 * Finds the next token in a line.
 * @param line The line to search for tokens.
 * @param token The buffer to store the found token.
 * @param del The delimiter character.
 * @return Returns the index of the next character after the token.
 */
int find_next_token(char *line, char *token, char del)
{
	int index = 0; // Index variable for traversing the line
	int i = 0;	   // Index variable for storing characters in the token buffer

	MOVE_TO_NOT_WHITE(line, index); // Move to the next non-white space character

	// Iterate until reaching the end of the line, a white space, the delimiter, or the maximum token size
	while (!is_end_of_line(line[index]) && !isspace(line[index]) && line[index] != del && i < LINESIZE + 1)
	{
		token[i++] = line[index++]; // Copy character to the token buffer and move to the next character
	}
	token[i] = '\0'; // Null-terminate the token buffer

	return index; // Return the index of the next character after the token
}

/**
 * Prints the corrsponding error message for a given error code.
 * @param error_code The given error's, error code.
 * @param line_num The corrsponding line number of the error.
 * @return Returns TRUE if the string represents an integer, FALSE otherwise.
 */
void print_error_message(error error_code, int line_num)
{
	fprintf(stderr, "line %d: ", line_num);
	switch (error_code)
	{
	case WARNING_LINE_TOO_LONG:
		fprintf(stderr, "Warning: Line too long.\n");
		break;
	case NUM_OUT_OF_RANGE:
		fprintf(stderr, "Error: Number out of range.\n");
		break;
	case MACRO_UNEXPECTED_CHARS:
		fprintf(stderr, "Error: Unexpected characters in macro.\n");
		break;
	case MACRO_TOO_LONG:
		fprintf(stderr, "Error: Macro is too long.\n");
		break;
	case MACRO_CANT_BE_EMPTY:
		fprintf(stderr, "Error: Macro cannot be empty.\n");
		break;
	case MACRO_INVALID_FIRST_CHAR:
		fprintf(stderr, "Error: Invalid first character in macro.\n");
		break;
	case MACRO_ONLY_PRINTABLE:
		fprintf(stderr, "Error: Macro must contain only printable characters.\n");
		break;
	case MACRO_CANT_BE_COMMAND:
		fprintf(stderr, "Error: Macro cannot be a command.\n");
		break;
	case MACRO_ALREADY_EXISTS:
		fprintf(stderr, "Error: Macro already exists.\n");
		break;
	case MACRO_CANT_BE_REGISTER:
		fprintf(stderr, "Error: Macro cannot be a register.\n");
		break;
	case MACRO_CANT_BE_INSTRUCT:
		fprintf(stderr, "Error: Macro cannot be an instruction.\n");
		break;
	case LABEL_TOO_LONG:
		fprintf(stderr, "Error: Label is too long.\n");
		break;
	case LABEL_INVALID_FIRST_CHAR:
		fprintf(stderr, "Error: Invalid first character in label.\n");
		break;
	case LABEL_ONLY_ALPHANUMERIC:
		fprintf(stderr, "Error: Label must contain only alphanumeric characters.\n");
		break;
	case LABEL_CANT_BE_COMMAND:
		fprintf(stderr, "Error: Label cannot be a command.\n");
		break;
	case LABEL_CANT_BE_MACRO:
		fprintf(stderr, "Error: Label cannot be a macro.\n");
		break;
	case LABEL_ALREADY_EXISTS:
		fprintf(stderr, "Error: Label already exists.\n");
		break;
	case WARNING_EMPTY_LABEL:
		fprintf(stderr, "Warning: Label is empty.\n");
		break;
	case LABEL_CANT_BE_REGISTER:
		fprintf(stderr, "Error: Label cannot be a register.\n");
		break;
	case LABEL_CANT_BE_INSTRUCT:
		fprintf(stderr, "Error: Label cannot be an instruction.\n");
		break;
	case DEFINE_EXPECTED_NUM:
		fprintf(stderr, "Error: Expected number after define.\n");
		break;
	case DEFINE_EXPECTED_EQUAL:
		fprintf(stderr, "Error: Define must have an equal sign.\n");
		break;
	case DEFINE_CANT_HAVE_LABEL:
		fprintf(stderr, "Error: Define cannot have a label.\n");
		break;
	case INSTRUCTION_NOT_FOUND:
		fprintf(stderr, "Error: Instruction not found.\n");
		break;
	case INSTRUCTION_INVALID_NUM_PARAMS:
		fprintf(stderr, "Error: Invalid number of parameters for instruction.\n");
		break;
	case DATA_EXPECTED_CONST:
		fprintf(stderr, "Error: Expected a number or a constant in data.\n");
		break;
	case DATA_EXPECTED_COMMA_AFTER_NUM:
		fprintf(stderr, "Error: Expected a comma after number in data.\n");
		break;
	case DATA_UNEXPECTED_COMMA:
		fprintf(stderr, "Error: Unexpected comma in data.\n");
		break;
	case DATA_LABEL_DOES_NOT_EXIST:
		fprintf(stderr, "Error: Label does not exist in data.\n");
		break;
	case STRING_TOO_MANY_OPERANDS:
		fprintf(stderr, "Error: Too many operands for string.\n");
		break;
	case STRING_UNEXPECTED_CHARS:
		fprintf(stderr, "Error: Unexpected characters in string.\n");
		break;
	case STRING_OPERAND_NOT_VALID:
		fprintf(stderr, "Error: Operand not valid in string.\n");
		break;
	case INVALID_ADDRESSING_TYPE:
		fprintf(stderr, "Error: Invalid addressing type.\n");
		break;
	case INDEX_EXPECTED_CLOSING_BRACKET:
		fprintf(stderr, "Error: Expected closing bracket for index.\n");
		break;
	case INDEX_INVALID_POSITION:
		fprintf(stderr, "Error: Invalid position for index.\n");
		break;
	case EXPECTED_COMMA_BETWEEN_OPERANDS:
		fprintf(stderr, "Error: Expected comma between operands.\n");
		break;
	case EXTERN_NO_LABEL:
		fprintf(stderr, "Error: Extern cannot be a label.\n");
		break;
	case EXTERN_INVALID_LABEL:
		fprintf(stderr, "Error: Invalid label in extern.\n");
		break;
	case EXTERN_TOO_MANY_OPERANDS:
		fprintf(stderr, "Error: Too many operands in extern.\n");
		break;
	case COMMAND_NOT_FOUND:
		fprintf(stderr, "Error: Command not found.\n");
		break;
	case COMMAND_UNEXPECTED_CHAR:
		fprintf(stderr, "Error: Unexpected character in command.\n");
		break;
	case COMMAND_TOO_MANY_OPERANDS:
		fprintf(stderr, "Error: Too many operands in command.\n");
		break;
	case COMMAND_INVALID_ADDRESSING:
		fprintf(stderr, "Error: Invalid type in command.\n");
		break;
	case COMMAND_INVALID_NUMBER_OF_OPERANDS:
		fprintf(stderr, "Error: Invalid number of operands in command.\n");
		break;
	case COMMAND_LABEL_DOES_NOT_EXIST:
		fprintf(stderr, "Error: Label does not exist in command.\n");
		break;
	case ENTRY_LABEL_DOES_NOT_EXIST:
		fprintf(stderr, "Error: Label does not exist in entry.\n");
		break;
	case ENTRY_TOO_MANY_OPERANDS:
		fprintf(stderr, "Error: Too many operands in entry.\n");
		break;
	case ENTRY_CANT_BE_EXTERN:
		fprintf(stderr, "Error: Entry cannot be extern.\n");
		break;
	case CANNOT_OPEN_FILE:
		fprintf(stderr, "Error: Cannot open file.\n");
		break;
	case FAILED_TO_CREATE_FILE:
		fprintf(stderr, "Error: Cannot create file.\n");
		break;
	case FAILED_TO_ALLOCATE_MEMORY:
		fprintf(stderr, "Error: Failed to allocate memory.\n");
		break;
	default:
		fprintf(stderr, "Unknown error code.\n");
		break;
	}
}
