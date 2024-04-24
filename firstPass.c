#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dataHandlers.h"
#include "cmdHandlers.h"
#include "vars.h"
#include "firstPass.h"

/**
 * Performs the first pass of the assembler, processing each line in the input file.
 * @param fp Pointer to the input file.
 */
void first_pass(FILE *fp)
{
    char line[LINESIZE + 2]; // Buffer to hold each line read from the file
    int line_num = 1;        // Line number counter

    ic = 0; // Initialize instruction counter
    dc = 0; // Initialize data counter

    has_error = FALSE; // Flag to indicate if an error has occurred

    // Loop through each line in the file
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        err = FALSE;  // Reset error flag for each line
        warn = FALSE; // Reset warning flag for each line

        // Process the current line
        if (!process_line(line))
        {                                       // If an error occurred while processing the line
            has_error = TRUE;                   // Set error flag
            print_error_message(err, line_num); // Print error message
        }

        // Print warning message if there was a warning
        if (warn)
        {
            print_error_message(warn, line_num);
        }

        line_num++; // Increment line number
    }

    // Adjust the data offset in the symbol table
    offset_data(&symbols, ic + RESERVED_MEMORY);
}

/**
 * Processes a line of assembly code during the first pass of the assembler.
 * @param line The line of assembly code to process.
 * @return TRUE if the line was processed successfully, FALSE otherwise.
 */
int process_line(char *line)
{
    instruction instruction;          // Variable to hold the detected instruction
    char symbol[SYMBOL_MAX_SIZE + 1]; // Buffer to store symbols
    int index = 0;                    // Index to track the position in the line

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(line, index);

    // Find and process label if present
    if (find_label(line, symbol) && !is_valid_symbol(symbol))
    {
        return FALSE; // Invalid symbol
    }
    if (symbol[0])
    {
        // Move index past the label
        for (; line[index] != ':'; index++)
            ;
        index++;
    }

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(line, index);

    // Check for an empty label
    if (symbol[0] && is_end_of_line(line[index]))
    {
        warn = WARNING_EMPTY_LABEL;
        return TRUE;
    }

    // Check if the symbol already exists in the symbol table
    if (findSymbol(&symbols, symbol))
    {
        err = LABEL_ALREADY_EXISTS;
        return FALSE;
    }

    // Find the instruction in the line
    instruction = find_instruction(line, &index);

    // Check for specific cases related to instructions and symbols
    if (instruction == DEFINE_IN && symbol[0] != '\0')
    {
        err = DEFINE_CANT_HAVE_LABEL;
        return FALSE;
    }
    if ((instruction == STRING_IN || instruction == DATA_IN) && symbol[0] != '\0')
    {
        addSymbol(&symbols, symbol, dc, DATA); // Add symbol to symbol table for data or string
    }

    // Move index to the next non-white space character
    MOVE_TO_NOT_WHITE(line, index);

    // Process the instruction or directive
    if (instruction != NONE_IN)
    {
        switch (instruction)
        {
        case DEFINE_IN:
            return defineHandler(&line[index]); // Handle define directive
        case STRING_IN:
            return stringHandler(&line[index]); // Handle string directive
        case DATA_IN:
            return dataHandler(&line[index]); // Handle data directive
        case EXTERN_IN:
            return externHandler(&line[index]); // Handle extern directive
        case ENTRY_IN:
            return entryValidator(&line[index]); // Handle entry directive
        }
    }

    // If a label exists, add it to the symbol table
    if (symbol[0])
    {
        addSymbol(&symbols, symbol, ic + RESERVED_MEMORY, CODE);
    }

    // Process if code
    return process_code(&line[index]);
}

/**
 * Processes the code portion of a line of assembly during the first pass of the assembler.
 * @param line The line of assembly code to process.
 * @return TRUE if the code portion of the line was processed successfully, FALSE otherwise.
 */
int process_code(char *line)
{
    opcode operation;               // Variable to hold the detected operation
    int index = 0;                  // Index to track the position in the line
    addressing_type first_operand;  // Addressing type of the first operand
    addressing_type second_operand; // Addressing type of the second operand
    int count = 0;                  // Counter for the number of operands

    // Find the operation in the line and update index
    operation = find_operation(line, &index);
    MOVE_TO_NOT_WHITE(line, index); // Move index to the next non-white space character

    // Check if operation is found
    if (operation == NONE_OP)
    {
        err = COMMAND_NOT_FOUND;
        return FALSE; // Operation not found, return false
    }

    // Handle operation and extract operands
    if (!operationHandler(&line[index], &first_operand, &second_operand))
    {
        return FALSE; // Error handling operation, return false
    }

    // Check if any operand is invalid
    if (first_operand == ERROR_ADDR || second_operand == ERROR_ADDR)
    {
        return FALSE; // Invalid operand, return false
    }

    // Count the number of operands
    count += first_operand != NONE_ADDR ? 1 : 0;
    count += second_operand != NONE_ADDR ? 1 : 0;

    // Validate the number of operands for the operation
    if (!validate_operand_count_by_opcode(operation, count))
    {
        err = COMMAND_INVALID_NUMBER_OF_OPERANDS;
        return FALSE; // Invalid number of operands, return false
    }

    // Validate the addressing modes for the operands
    if (!command_accept_methods(operation, first_operand, second_operand))
    {
        err = COMMAND_INVALID_ADDRESSING;
        return FALSE; // Invalid addressing mode, return false
    }

    // Insert the first word into the instructions list
    insert_instructions(build_first_word(operation, first_operand, second_operand));

    // Update the instruction counter by the number of additional words required
    ic += calculate_command_num_additional_words(first_operand, second_operand);

    return TRUE; // operation processed successfully, return true
}

/**
 * Builds the first word of the instruction based on the opcode and addressing modes of operands.
 * @param operation The opcode of the instruction.
 * @param first_operand The addressing mode of the first operand.
 * @param second_operand The addressing mode of the second operand.
 * @return The first word of the instruction.
 */
unsigned int build_first_word(opcode operation, addressing_type first_operand, addressing_type second_operand)
{
    unsigned int word = 0; // Initialize word to hold the first word of the instruction

    /* Inserting the opcode */
    word = (unsigned int)operation; // Insert the opcode into the word
    word <<= BITS_IN_ADDRESSING;    // Leave space for the first addressing method

    /* If there are two operands, insert the first */
    if (first_operand != NONE_ADDR && second_operand != NONE_ADDR)
    {
        word |= (unsigned int)first_operand; // Insert the first addressing method
    }

    word <<= BITS_IN_ADDRESSING; // Leave space for the second addressing method

    /* If there are two operands, insert the second. */
    if (first_operand != NONE_ADDR && second_operand != NONE_ADDR)
    {
        word |= (unsigned int)second_operand; // Insert the second addressing method
    }
    /* If not, insert the first one (a single operand is a destination operand). */
    else if (first_operand != NONE_ADDR)
    {
        word |= (unsigned int)first_operand; // Insert the addressing method for the single operand
    }

    word = insert_are(word, ABSOLUTE); // Insert A/R/E mode to the word

    return word; // Return the built first word of the instruction
}

/**
 * Determines the number of words required for encoding an operand based on its addressing type.
 * @param operand The addressing type of the operand.
 * @return The number of words required.
 */
int num_words(addressing_type operand)
{
    // Switch statement to handle different addressing types
    switch (operand)
    {
    // For immediate, direct, and register addressing, only one word is required
    case IMMEDIATE_ADDR:
    case DIRECT_ADDR:
    case REGISTER_ADDR:
        return 1;
    // For index addressing, two words are required
    case INDEX_ADDR:
        return 2;
    }
    // Return 0 if the addressing type is not recognized
    return 0;
}

/**
 * Calculates the total number of additional words required for encoding operands of a command.
 * @param first_operand The addressing type of the first operand.
 * @param second_operand The addressing type of the second operand.
 * @return The total number of additional words required.
 */
int calculate_command_num_additional_words(addressing_type first_operand, addressing_type second_operand)
{
    int count = 0; // Initialize count to hold the total number of additional words

    // If the first operand exists, add the number of words required for it
    if (first_operand != NONE_ADDR)
        count += num_words(first_operand);

    // If the second operand exists, add the number of words required for it
    if (second_operand != NONE_ADDR)
        count += num_words(second_operand);

    // If both operands are registers, they will share the same additional word
    if (first_operand == REGISTER_ADDR && second_operand == REGISTER_ADDR)
        count--; // Decrement count by 1 to avoid double counting the shared word

    return count; // Return the total number of additional words required
}

/**
 * Checks if the given addressing modes are acceptable for the specified opcode.
 * @param type The opcode type.
 * @param first_operand The addressing type of the first operand.
 * @param second_operand The addressing type of the second operand.
 * @return TRUE if the addressing modes are acceptable for the opcode, otherwise FALSE.
 */
int command_accept_methods(opcode type, addressing_type first_operand, addressing_type second_operand)
{
    switch (type)
    {
    /* These opcodes only accept
     * Src: 0, 1, 2, 3
     * Dst 1, 2, 3
     */
    case MOV_OP:
    case ADD_OP:
    case SUB_OP:
        return ((first_operand >= IMMEDIATE_ADDR && first_operand <= REGISTER_ADDR) ||
                first_operand == INDEX_ADDR) &&
               (second_operand >= DIRECT_ADDR && second_operand <= REGISTER_ADDR);

    /* LEA opcode only accept
     * Src: 1, 2
     * Dst: 1, 2, 3
     */
    case LEA_OP:
        return ((first_operand == DIRECT_ADDR || first_operand == INDEX_ADDR) &&
                (second_operand >= DIRECT_ADDR && second_operand <= REGISTER_ADDR));

    /* These opcodes only accept
     * Source: NONE
     * Destination: 1, 2, 3
     */
    case NOT_OP:
    case CLR_OP:
    case INC_OP:
    case DEC_OP:
    case JMP_OP:
    case BNE_OP:
    case RED_OP:
    case JSR_OP:
        return (first_operand == DIRECT_ADDR ||
                first_operand == INDEX_ADDR ||
                first_operand == REGISTER_ADDR);

    // These opcodes accept any or no methods.
    case PRN_OP:
    case RTS_OP:
    case HLT_OP:
    case CMP_OP:
        return TRUE;
    }
    return FALSE;
}
