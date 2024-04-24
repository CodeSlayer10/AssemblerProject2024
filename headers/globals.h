
#ifndef _GLOBALS_H
#define _GLOBALS_H

#define MAX_MEMORY_SIZE 4096 // Maximum memory size
#define LINESIZE 80          // Maximum line size
#define SYMBOL_MAX_SIZE 31   // Maximum size of a symbol
#define RESERVED_MEMORY 100  // Reserved memory space
#define FALSE 0              // False value
#define TRUE 1               // True value

/* Bit-related info */
#define BITS_IN_WORD 14      // Number of bits in a word
#define BITS_IN_OPCODE 4     // Number of bits for opcode
#define BITS_IN_ADDRESSING 2 // Number of bits for addressing
#define BITS_IN_ARE 2        // Number of bits for ARE
#define BITS_IN_REGISTER 3   // Number of bits for register

/* Addressing methods bits location in the first word of a command */
#define SRC_TYPE_START_POS 4
#define SRC_TYPE_END_POS 5
#define DST_TYPE_START_POS 2
#define DST_TYPE_END_POS 3


#define BASE4_SIZE 8 // Size of base-4 representation

typedef enum ARE
{
    ABSOLUTE,   // Absolute addressing mode
    EXTERN,     // External addressing mode
    RELOCATABLE // Relocatable addressing mode
} ARE;

typedef enum opcode
{
    MOV_OP,
    CMP_OP,
    ADD_OP,
    SUB_OP,
    NOT_OP,
    CLR_OP,
    LEA_OP,
    INC_OP,
    DEC_OP,
    JMP_OP,
    BNE_OP,
    RED_OP,
    PRN_OP,
    JSR_OP,
    RTS_OP,
    HLT_OP,
    NONE_OP,
    ERROR_OP
} opcode;

typedef enum reg
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R_NONE
} reg;

typedef enum attribute
{
    MDEFINE,  // Macro definition attribute
    CODE,     // Code attribute
    DATA,     // Data attribute
    EXTERNAL, // External attribute
    ENTRY,    // Entry attribute
    NONE,     // No attribute
    ERROR     // Error attribute
} attribute;
/*
ERROR wasn't used in the end,
but the idea was that if a symbol
as valid but not correctly defined
or empty it would be added with an ERROR attr
and there would only be one error instead of per use.
*/
typedef enum instruction
{
    DEFINE_IN, // Define instruction
    STRING_IN, // String instruction
    DATA_IN,   // Data instruction
    EXTERN_IN, // External instruction
    ENTRY_IN,  // Entry instruction
    NONE_IN,   // No instruction
    ERROR_IN   // Error instruction
} instruction;

typedef enum addressing_type
{
    IMMEDIATE_ADDR, // Immediate addressing mode
    DIRECT_ADDR,    // Direct addressing mode
    INDEX_ADDR,     // Index addressing mode
    REGISTER_ADDR,  // Register addressing mode
    NONE_ADDR,      // No addressing mode
    ERROR_ADDR      // Error addressing mode
} addressing_type;

typedef enum FILE_TYPE
{
    AS_FILE,  // Assembly source file
    AM_FILE,  // Assembled machine code file
    OB_FILE,  // Object file
    ENT_FILE, // Entry file
    EXT_FILE  // External file
} FILE_TYPE;

// Error codes for various errors encountered in the program for error handling.
typedef enum errors
{
    WARNING_LINE_TOO_LONG = 1,
    NUM_OUT_OF_RANGE,
    MACRO_UNEXPECTED_CHARS,
    MACRO_TOO_LONG,
    MACRO_CANT_BE_EMPTY,
    MACRO_INVALID_FIRST_CHAR,
    MACRO_ONLY_PRINTABLE,
    MACRO_CANT_BE_COMMAND,
    MACRO_ALREADY_EXISTS,
    MACRO_CANT_BE_REGISTER,
    MACRO_CANT_BE_INSTRUCT,
    LABEL_TOO_LONG,
    LABEL_INVALID_FIRST_CHAR,
    LABEL_ONLY_ALPHANUMERIC,
    LABEL_CANT_BE_COMMAND,
    LABEL_CANT_BE_MACRO,
    LABEL_ALREADY_EXISTS,
    WARNING_EMPTY_LABEL,
    LABEL_CANT_BE_REGISTER,
    LABEL_CANT_BE_INSTRUCT,
    DEFINE_EXPECTED_NUM,
    DEFINE_EXPECTED_EQUAL,
    DEFINE_CANT_HAVE_LABEL,
    INSTRUCTION_NOT_FOUND,
    INSTRUCTION_INVALID_NUM_PARAMS,
    DATA_EXPECTED_CONST,
    DATA_EXPECTED_COMMA_AFTER_NUM,
    DATA_UNEXPECTED_COMMA,
    DATA_LABEL_DOES_NOT_EXIST,
    STRING_TOO_MANY_OPERANDS,
    STRING_UNEXPECTED_CHARS,
    STRING_OPERAND_NOT_VALID,
    INVALID_ADDRESSING_TYPE,
    INDEX_EXPECTED_CLOSING_BRACKET,
    INDEX_INVALID_POSITION,
    EXPECTED_COMMA_BETWEEN_OPERANDS,
    EXTERN_NO_LABEL,
    EXTERN_INVALID_LABEL,
    EXTERN_TOO_MANY_OPERANDS,
    COMMAND_NOT_FOUND,
    COMMAND_UNEXPECTED_CHAR,
    COMMAND_TOO_MANY_OPERANDS,
    COMMAND_INVALID_ADDRESSING,
    COMMAND_INVALID_NUMBER_OF_OPERANDS,
    COMMAND_LABEL_DOES_NOT_EXIST,
    ENTRY_LABEL_DOES_NOT_EXIST,
    ENTRY_TOO_MANY_OPERANDS,
    ENTRY_CANT_BE_EXTERN,
    CANNOT_OPEN_FILE,
    FAILED_TO_CREATE_FILE,
    FAILED_TO_ALLOCATE_MEMORY
} error;
#endif
