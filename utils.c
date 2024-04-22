#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "utils.h"
#include "vars.h"

struct operationLookupItem
{
	char *name;
	opcode operation;
	int operands;
} operationLookupTable[] = {
	{"mov", MOV_OP, 2},
	{"cmp", CMP_OP, 2},
	{"add", ADD_OP, 2},
	{"sub", SUB_OP, 2},
	{"not", NOT_OP, 1},
	{"clr", CLR_OP, 1},
	{"lea", LEA_OP, 2},
	{"inc", INC_OP, 1},
	{"dec", DEC_OP, 1},
	{"jmp", JMP_OP, 1},
	{"bne", BNE_OP, 1},
	{"red", RED_OP, 1},
	{"prn", PRN_OP, 1},
	{"jsr", JSR_OP, 1},
	{"rts", RTS_OP, 0},
	{"hlt", HLT_OP, 0},
	{NULL, NONE_OP, 0},
};

struct instructionLookupItem
{
	char *name;
	instruction instruction;
	attribute attribute;
} instructionLookupTable[] = {
	{".define", DEFINE_IN, MDEFINE},
	{".string", STRING_IN, DATA},
	{".data", DATA_IN, DATA},
	{".entry", ENTRY_IN, ENTRY},
	{".extern", EXTERN_IN, EXTERNAL},
	{NULL, NONE_IN, NONE},
};

struct registerLookupItem
{
	char *name;
	reg reg;
} registerLookupTable[] = {
	{"r0", R0},
	{"r1", R1},
	{"r2", R2},
	{"r3", R3},
	{"r4", 44},
	{"r5", R5},
	{"r6", R6},
	{"r7", R7},
	{NULL, R_NONE},
};

// allocating and concatenate
char *strallocat(char *s0, char *s1)
{
	char *str = (char *)checkedAlloc(strlen(s0) + strlen(s1) + 1);
	strcpy(str, s0);
	strcat(str, s1);
	return str;
}

// malloc with NULL check
void *checkedAlloc(long size)
{
	void *ptr = malloc(size);
	if (ptr == NULL)
	{
		reset_global_vars();
		printf("Error: Memory allocation failed.");
	}
	return ptr;
}

int is_reserved(char *name, int is_label)
{
	if (find_instruction_by_name(name) != NONE_IN)
	{
		err = is_label ? LABEL_CANT_BE_INSTRUCT : MACRO_CANT_BE_INSTRUCT;

		return TRUE;
	}

	if (find_register_by_name(name) != R_NONE)
	{
		err = is_label ? LABEL_CANT_BE_REGISTER : MACRO_CANT_BE_REGISTER;
		return TRUE;
	}

	if (find_operation_by_name(name) != NONE_OP)
	{
		err = is_label ? LABEL_CANT_BE_COMMAND : MACRO_CANT_BE_COMMAND;

		return TRUE;
	}
	return FALSE;
}

instruction find_instruction_by_name(char *name)
{
	int i = 0;
	char *current;
	while ((current = instructionLookupTable[i].name) != NULL)
	{
		if (strcmp(current, name) == 0)
			return instructionLookupTable[i].instruction;
		i++;
	}
	return NONE_IN;
}

reg find_register_by_name(char *name)
{
	int i = 0;
	char *current;
	while ((current = registerLookupTable[i].name) != NULL)
	{
		if (strcmp(current, name) == 0)
			return registerLookupTable[i].reg;
		i++;
	}
	return R_NONE;
}

opcode find_operation_by_name(char *name)
{
	int i = 0;
	char *current;
	while ((current = operationLookupTable[i].name) != NULL)
	{
		if (strcmp(current, name) == 0)
			return operationLookupTable[i].operation;
		i++;
	}
	return NONE_OP;
}

int validate_operand_count_by_opcode(opcode operation, int count)
{
	return count == operationLookupTable[operation].operands;
}
int get_operand_count_by_opcode(opcode operation)
{
	return operationLookupTable[operation].operands;
}

int is_alphanum_str(char *str)
{
	int i = 0;
	for (; str[i] && isalnum(str[i]); i++)
		;
	if (i == 0 || str[i] != '\0')
	{
		return FALSE;
	}
	return TRUE;
}

int is_int_str(char *str)
{
	int i = 0;
	if (str[0] == '+' || str[0] == '-')
		str++;
	for (; str[i] && isdigit(str[i]); i++)
		;
	if (i == 0 || str[i] != '\0')
	{
		return FALSE;
	}
	return TRUE;
}

int is_printable_str(char *str)
{
	int i = 0;
	for (; str[i] && isprint(str[i]); i++)
		;
	if (i == 0 || str[i] != '\0')
	{
		return FALSE;
	}
	return TRUE;
}

int is_valid_symbol(char *symbol)
{
	// Check if the first character of the symbol is not '\0' (non-empty string)
	if (!symbol[0])
	{
		err = LABEL_CANT_BE_EMPTY;
		return FALSE;
	}

	if (!isalpha(symbol[0]))
	{
		err = LABEL_INVALID_FIRST_CHAR;

		return FALSE;
	}

	// Check if the length of the symbol is greater than the allowed maximum size
	if (strlen(symbol) > SYMBOL_MAX_SIZE)
	{
		err = LABEL_TOO_LONG;

		return FALSE;
	}

	// Check if the symbol is reserved
	if (is_reserved(symbol, TRUE))
	{
		return FALSE;
	}

	// Check if the symbol is alphanumeric
	if (!is_alphanum_str(symbol))
	{
		err = LABEL_ONLY_ALPHANUMERIC;
		return FALSE;
	}

	if (lookup(macroTable, symbol) != NULL) {
		err = LABEL_CANT_BE_MACRO;
		return FALSE;
	}

	// If all conditions are met, return true
	return TRUE;
}

int is_valid_macro(char *macro)
{

	// Check if the first character of the macro is not '\0' (non-empty string)
	if (!macro[0])
	{
		err = MACRO_CANT_BE_EMPTY;
		return FALSE;
	}

	// Check if the first character is an alphabetic character
	if (!isalpha(macro[0]))
	{
		err = MACRO_INVALID_FIRST_CHAR;
		return FALSE;
	}

	// Check if all characters in the macro are printable
	if (!is_printable_str(macro))
	{
		err = MACRO_ONLY_PRINTABLE;
		return FALSE;
	}

	// Check if the macro is reserved
	if (is_reserved(macro, FALSE))
	{
		return FALSE;
	}

	// If all conditions are met, return true
	return TRUE;
}


int is_end_of_line(char chr)
{
	return chr == '\0' || chr == '\n';
}

int insert_data(int num)
{
	if (ic + dc + RESERVED_MEMORY > MAX_MEMORY_SIZE)
	{
		err = FAILED_TO_ALLOCATE_MEMORY;
		return FALSE;
	}
	data[dc++] = (unsigned int)num;
	return TRUE;
}

int insert_instructions(int num)
{
	if (ic + dc + RESERVED_MEMORY > MAX_MEMORY_SIZE)
	{
		err = FAILED_TO_ALLOCATE_MEMORY;
		return FALSE;
	}
	instructions[ic++] = (unsigned int)num;
	return TRUE;
}
unsigned int insert_are(unsigned int info, ARE are)
{
	return (info << BITS_IN_ARE) | are; /* OR operand allows insertion of the 2 bits because 1 + 0 = 1 */
}

int is_in_range(int num)
{
	// Calculate the minimum and maximum values of a signed 12-bit integer
	int min_val = -(1 << 11);
	int max_val = (1 << 11) - 1;

	// Check if the number is within the range
	return (num >= min_val && num <= max_val);
}

/* This function extracts bits, given start and end positions of the bit-sequence (0 is LSB) */
unsigned int extract_bits(unsigned int word, int start, int end)
{
	unsigned int result;
	int length = end - start + 1;				 /* Length of bit-sequence */
	unsigned int mask = (int)pow(2, length) - 1; /* Creating a '111...1' mask with above line's length */

	mask <<= start;		  /* Moving mask to place of extraction */
	result = word & mask; /* The bits are now in their original position, and the rest is 0's */
	result >>= start;	  /* Moving the sequence to LSB */
	return result;
}

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

	return base4_tmp;
}

void reset_global_vars()
{
	resetTable(macroTable);
	resetSymbolTable(&symbols);
	reset_ext(&externals);
	has_entry = FALSE;
	has_external = FALSE;
	has_error = FALSE;
	err = FALSE;
}

int find_next_symbol(char *line, char *symbol, char del)
{
	int index = 0;
	int i = 0;

	MOVE_TO_NOT_WHITE(line, index);

	while (!is_end_of_line(line[index]) && !isspace(line[index]) && line[index] != del && i < SYMBOL_MAX_SIZE)
	{
		symbol[i++] = line[index++];
	}
	symbol[i] = '\0';

	return index;
}

int find_next_token(char *line, char *token, char del)
{
	int index = 0;
	int i = 0;

	MOVE_TO_NOT_WHITE(line, index);

	while (!is_end_of_line(line[index]) && !isspace(line[index]) && line[index] != del && i < LINESIZE + 1)
	{
		token[i++] = line[index++];
	}
	token[i] = '\0';

	return index;
}

void print_error_message(error error_code, int line_num)
{
	fprintf(stderr, "line %d: ", line_num);
	switch (error_code)
	{
	case WARNING_LINE_TOO_LONG:
		fprintf(stderr, "Warning: Line too long.\n");
		break;
	case NUM_OUT_OF_RANGE:
		fprintf(stderr, "Number out of range.\n");
		break;
	case MACRO_UNEXPECTED_CHARS:
		fprintf(stderr, "Unexpected characters in macro.\n");
		break;
	case MACRO_TOO_LONG:
		fprintf(stderr, "Macro is too long.\n");
		break;
	case MACRO_CANT_BE_EMPTY:
		fprintf(stderr, "Macro cannot be empty.\n");
		break;
	case MACRO_INVALID_FIRST_CHAR:
		fprintf(stderr, "Invalid first character in macro.\n");
		break;
	case MACRO_ONLY_PRINTABLE:
		fprintf(stderr, "Macro must contain only printable characters.\n");
		break;
	case MACRO_CANT_BE_COMMAND:
		fprintf(stderr, "Macro cannot be a command.\n");
		break;
	case MACRO_ALREADY_EXISTS:
		fprintf(stderr, "Macro already exists.\n");
		break;
	case MACRO_CANT_BE_REGISTER:
		fprintf(stderr, "Macro cannot be a register.\n");
		break;
	case MACRO_CANT_BE_INSTRUCT:
		fprintf(stderr, "Macro cannot be an instruction.\n");
		break;
	case LABEL_TOO_LONG:
		fprintf(stderr, "Label is too long.\n");
		break;
	case LABEL_CANT_BE_EMPTY:
		fprintf(stderr, "Label cannot be empty.\n");
		break;
	case LABEL_INVALID_FIRST_CHAR:
		fprintf(stderr, "Invalid first character in label.\n");
		break;
	case LABEL_ONLY_ALPHANUMERIC:
		fprintf(stderr, "Label must contain only alphanumeric characters.\n");
		break;
	case LABEL_CANT_BE_COMMAND:
		fprintf(stderr, "Label cannot be a command.\n");
		break;
	case LABEL_ALREADY_EXISTS:
		fprintf(stderr, "Label already exists.\n");
		break;
	case WARNING_LABEL_ONLY:
		fprintf(stderr, "Warning: Label only.\n");
		break;
	case LABEL_CANT_BE_REGISTER:
		fprintf(stderr, "Label cannot be a register.\n");
		break;
	case LABEL_CANT_BE_INSTRUCT:
		fprintf(stderr, "Label cannot be an instruction.\n");
		break;
	case DEFINE_EXPECTED_NUM:
		fprintf(stderr, "Expected number after define.\n");
		break;
	case DEFINE_CANT_HAVE_LABEL:
		fprintf(stderr, "Define cannot have a label.\n");
		break;
	case INSTRUCTION_NOT_FOUND:
		fprintf(stderr, "Instruction not found.\n");
		break;
	case INSTRUCTION_NO_PARAMS:
		fprintf(stderr, "Instruction does not take any parameters.\n");
		break;
	case INSTRUCTION_INVALID_NUM_PARAMS:
		fprintf(stderr, "Invalid number of parameters for instruction.\n");
		break;
	case DATA_COMMAS_IN_A_ROW:
		fprintf(stderr, "Commas in a row in data.\n");
		break;
	case DATA_EXPECTED_CONST:
		fprintf(stderr, "Expected a num or a constant in data.\n");
		break;
	case DATA_EXPECTED_COMMA_AFTER_NUM:
		fprintf(stderr, "Expected comma after number in data.\n");
		break;
	case DATA_UNEXPECTED_COMMA:
		fprintf(stderr, "Unexpected comma in data.\n");
		break;
	case DATA_LABEL_DOES_NOT_EXIST:
		fprintf(stderr, "Label does not exist in data.\n");
		break;
	case STRING_TOO_MANY_OPERANDS:
		fprintf(stderr, "Too many operands for string.\n");
		break;
	case STRING_UNEXPECTED_CHARS:
		fprintf(stderr, "Unexpected characters in string.\n");
		break;
	case STRING_OPERAND_NOT_VALID:
		fprintf(stderr, "Operand not valid in string.\n");
		break;
	case INVALID_ADDRESSING_TYPE:
		fprintf(stderr, "Invalid addressing type.\n");
		break;
	case INDEX_EXPECTED_CLOSING_BRACKET:
		fprintf(stderr, "Expected closing bracket for index.\n");
		break;
	case INDEX_INVALID_POSITION:
		fprintf(stderr, "Invalid position for index.\n");
		break;
	case EXPECTED_COMMA_BETWEEN_OPERANDS:
		fprintf(stderr, "Expected comma between operands.\n");
		break;
	case EXTERN_NO_LABEL:
		fprintf(stderr, "Extern cannot be a label.\n");
		break;
	case EXTERN_INVALID_LABEL:
		fprintf(stderr, "Invalid label in extern.\n");
		break;
	case EXTERN_TOO_MANY_OPERANDS:
		fprintf(stderr, "Too many operands in extern.\n");
		break;
	case COMMAND_NOT_FOUND:
		fprintf(stderr, "Command not found.\n");
		break;
	case COMMAND_UNEXPECTED_CHAR:
		fprintf(stderr, "Unexpected character in command.\n");
		break;
	case COMMAND_TOO_MANY_OPERANDS:
		fprintf(stderr, "Too many operands in command.\n");
		break;
	case COMMAND_INVALID_ADDRESSING:
		fprintf(stderr, "Invalid type in command.\n");
		break;
	case COMMAND_INVALID_NUMBER_OF_OPERANDS:
		fprintf(stderr, "Invalid number of operands in command.\n");
		break;
	case COMMAND_LABEL_DOES_NOT_EXIST:
		fprintf(stderr, "Label does not exist in command.\n");
		break;
	case ENTRY_LABEL_DOES_NOT_EXIST:
		fprintf(stderr, "Label does not exist in entry.\n");
		break;
	case ENTRY_TOO_MANY_OPERANDS:
		fprintf(stderr, "Too many operands in entry.\n");
		break;
	case ENTRY_CANT_BE_EXTERN:
		fprintf(stderr, "Entry cannot be extern.\n");
		break;
	case CANNOT_OPEN_FILE:
		fprintf(stderr, "Cannot open file.\n");
		break;
	case FAILED_TO_ALLOCATE_MEMORY:
		fprintf(stderr, "Failed to allocate memory.\n");
		break;
	default:
		fprintf(stderr, "Unknown error code.\n");
		break;
	}
}
