#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"
#include "dataHandlers.h"
#include "cmdHandlers.h"
#include "vars.h"
#include "secondPass.h"

/**
 * Second pass of the assembler.
 * @param fp Pointer to the input file.
 */
void second_pass(FILE *fp)
{
    char line[LINESIZE + 2]; // Buffer to hold each line read from the file
    int line_num = 1;        // Line number counter

    ic = 0;            // Initialize instruction counter
    has_error = FALSE; // Flag to indicate if an error has occurred

    // Loop through each line in the file
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        err = FALSE;  // Reset error flag for each line
        warn = FALSE; // Reset warning flag for each line

        // Process the current line in the second pass
        if (!process_line_second_pass(line))
        {
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
}

/**
 * Processes a line in the second pass of the assembler.
 * @param line The line to be processed.
 * @return TRUE if the line is successfully processed, FALSE otherwise.
 */
int process_line_second_pass(char *line)
{
    instruction instruction;
    opcode operation;
    char symbol[SYMBOL_MAX_SIZE + 1];
    int index = 0;

    // Move index to the first non-white space character
    MOVE_TO_NOT_WHITE(line, index);

    // Find label if present
    find_label(line, symbol);

    // Skip label if found
    if (symbol[0])
    {
        for (; line[index] != ':'; index++)
            ;
        index++;
    }

    // Move index to the first non-white space character after label
    MOVE_TO_NOT_WHITE(line, index);

    // Check for end of line
    if (is_end_of_line(line[index]))
    {
        return TRUE; // Return TRUE if end of line is reached
    }

    // Find instruction
    instruction = find_instruction(line, &index);

    // Move index to the first non-white space character after instruction
    MOVE_TO_NOT_WHITE(line, index);

    // Handle instruction if found
    if (instruction != NONE_IN)
    {
        if (instruction == ENTRY_IN)
        {
            return entryHandler(&line[index]); // Handle entry instruction
        }
        return TRUE; // Return TRUE if instruction is found and handled successfully
    }

    // Find operation
    operation = find_operation(line, &index);

    // Move index to the first non-white space character after operation
    MOVE_TO_NOT_WHITE(line, index);

    // Process operation if found
    if (operation != NONE_OP)
    {
        return process_operation(operation, &line[index]); // Process operation
    }
}

/**
 * Processes an operation with its operands.
 * @param operation The opcode of the operation to be processed.
 * @param args The arguments string containing operands.
 * @return TRUE if the operation is successfully processed, FALSE otherwise.
 */
int process_operation(opcode operation, char *args)
{
    int index = 0;
    char first_operand[LINESIZE], second_operand[LINESIZE];                     // Hold operands
    char *src = NULL, *dst = NULL;                                              // Pointers to operands
    int count = get_operand_count_by_opcode(operation);                         // Number of operands
    addressing_type src_operand_type = NONE_ADDR, dst_operand_type = NONE_ADDR; // Operand types

    // Extract operand types based on the count of operands
    if (count == 1 || count == 2)
    {
        dst_operand_type = extract_bits(instructions[ic], DST_TYPE_START_POS, DST_TYPE_END_POS);
    }
    if (count == 2)
    {
        src_operand_type = extract_bits(instructions[ic], SRC_TYPE_START_POS, SRC_TYPE_END_POS);
    }

    // Extract operands from the argument string
    index += find_next_symbol(args, first_operand, ',');
    index++;
    index += find_next_symbol(&args[index], second_operand, ',');

    // Set src and dst pointers based on the number of operands
    if (count >= 1)
    {
        dst = first_operand; // First operand is destination
    }
    if (count == 2)
    {
        src = first_operand;  // Second operand is source
        dst = second_operand; // First operand becomes destination
    }

    ic++; // Increment instruction count

    // Encode additional words based on operands and their types
    return encode_additional_words(src, dst, src_operand_type, dst_operand_type);
}

/**
 * Encodes additional words based on source and destination operands.
 * @param src_operand Pointer to the source operand.
 * @param dst_operand Pointer to the destination operand.
 * @param src_type The addressing type of the source operand.
 * @param dst_type The addressing type of the destination operand.
 * @return TRUE if the additional words are successfully encoded, FALSE otherwise.
 */
int encode_additional_words(char *src_operand, char *dst_operand, addressing_type src_type, addressing_type dst_type)
{
    int is_valid = TRUE; // Flag to indicate if encoding is valid

    // If both operands are register addresses
    if (src_type == REGISTER_ADDR && dst_type == REGISTER_ADDR)
    {
        // Build and insert register word
        insert_instructions(build_register_word(FALSE, src_operand) | build_register_word(TRUE, dst_operand));
    }
    else
    {
        // Encode additional words for source and destination operands
        if (src_operand)
        {
            is_valid = encode_additional_word(FALSE, src_type, src_operand); // Encode additional word for source operand
        }
        if (dst_operand)
        {
            is_valid = encode_additional_word(TRUE, dst_type, dst_operand); // Encode additional word for destination operand
        }
    }

    return is_valid; // Return flag indicating if encoding is valid
}

/**
 * Builds a register word based on whether it is for the destination operand or not.
 * @param is_dst Flag indicating if the register word is for the destination operand.
 * @param reg Pointer to the register string (e.g., "r1").
 * @return The built register word.
 */
unsigned int build_register_word(int is_dst, char *reg)
{
    unsigned int word = (unsigned int)atoi(reg + 1); // Extract register number
    if (!is_dst)
        word <<= BITS_IN_REGISTER;     // Shift register number to the left by BITS_IN_REGISTER if not for destination operand
    word = insert_are(word, ABSOLUTE); // Insert Absolute relocation attribute
    return word;                       // Return the built register word
}

/**
 * Encodes a label symbol.
 * @param symbol The name of the label symbol.
 * @return TRUE if the label symbol is successfully encoded, FALSE otherwise.
 */
int encode_label(char *symbol)
{
    unsigned int word = 0;                              // Initialize word to store encoded value
    Symbol *symbol_info = findSymbol(&symbols, symbol); // Find symbol information in the symbol table
    if (symbol_info == NULL)
    {
        ic++;                               // Increment instruction counter
        err = COMMAND_LABEL_DOES_NOT_EXIST; // Set error flag for non-existent label
        return FALSE;                       // Return FALSE indicating failure
    }
    word = (unsigned int)symbol_info->value; // Get value of the label symbol
    switch (symbol_info->attribute)
    {
    case MDEFINE:
        word = insert_are(word, ABSOLUTE); // Insert Absolute relocation attribute for MDEFINE symbol
        break;
    case EXTERNAL:
        add_ext(&externals, symbol, ic); // Add to external symbol table
        word = insert_are(word, EXTERN); // Insert External relocation attribute for EXTERNAL symbol
        break;
    default:
        word = insert_are(word, RELOCATABLE); // Insert Relocatable relocation attribute for other symbols
    }
    insert_instructions(word); // Insert encoded value into instructions array
    return TRUE;               // Return TRUE indicating success
}

/**
 * Encodes an additional word based on the addressing type and operand.
 * @param is_dst Flag indicating if the additional word is for the destination operand.
 * @param type The addressing type of the operand.
 * @param operand Pointer to the operand string.
 * @return TRUE if the additional word is successfully encoded, FALSE otherwise.
 */
int encode_additional_word(int is_dst, addressing_type type, char *operand)
{
    unsigned int word = 0; // Initialize word to store encoded value
    int is_valid = TRUE; // Flag indicating if encoding is valid

    // Choose encoding method based on addressing type
    switch (type)
    {
    case IMMEDIATE_ADDR:
        is_valid = handle_immediate_address(operand, &word); // Handle immediate address encoding
        break;

    case DIRECT_ADDR:
        is_valid = encode_label(operand); // Encode label for direct address
        break;

    case INDEX_ADDR:
        is_valid = handle_index_address(operand, &word); // Handle index address encoding
        break;

    case REGISTER_ADDR:
        word = build_register_word(is_dst, operand); // Build register word for register address
        insert_instructions(word); // Insert register word into instructions array
        break;
    }

    return is_valid; // Return flag indicating if encoding is valid
}

/**
 * Handles encoding of an immediate address operand.
 * @param operand Pointer to the operand string.
 * @param word Pointer to store the encoded value.
 * @return TRUE if the immediate address operand is successfully encoded, FALSE otherwise.
 */
int handle_immediate_address(char *operand, unsigned int *word)
{
    if (is_int_str(&operand[1])) // Check if operand is a valid integer
    {
        *word = (unsigned int)atoi(&operand[1]); // Convert operand to integer
        insert_are(*word, ABSOLUTE); // Insert Absolute relocation attribute
        insert_instructions(*word); // Insert encoded value into instructions
        return TRUE; // Return TRUE indicating successful encoding
    }
    else
    {
        return encode_label(&operand[1]); // Encode label for immediate address operand
    }
}


/**
 * Handles encoding of an index address operand.
 * @param operand Pointer to the operand string.
 * @param word Pointer to store the encoded value.
 * @return TRUE if the index address operand is successfully encoded, FALSE otherwise.
 */
int handle_index_address(char *operand, unsigned int *word)
{
    char *opening_bracket;
    char *closing_bracket;
    int is_valid;
    opening_bracket = strchr(operand, '['); // Find opening bracket
    closing_bracket = strchr(operand, ']'); // Find closing bracket
    *opening_bracket = '\0'; // Replace opening bracket with null terminator
    is_valid = encode_label(operand); // Encode label before opening bracket
    *opening_bracket = '['; // Restore opening bracket
    opening_bracket++; // Move to next character after opening bracket
    *closing_bracket = '\0'; // Replace closing bracket with null terminator
    if (is_int_str(opening_bracket)) // Check if string after opening bracket is a valid integer
    {
        *word = (unsigned int)atoi(opening_bracket); // Convert string to integer
        *word = insert_are(*word, ABSOLUTE); // Insert Absolute relocation attribute
        insert_instructions(*word); // Insert encoded value into instructions array
    }
    else
    {
        if (is_valid) // If label before opening bracket was successfully encoded
        {
            is_valid = encode_label(opening_bracket); // Encode label after opening bracket
        }
        else
        {
            encode_label(opening_bracket); // Encode label after opening bracket without checking validity
        }
    }
    *closing_bracket = ']'; // Restore closing bracket
    return is_valid; // Return flag indicating if encoding is valid
}
