# Makefile for Assembler project

# Compiler settings
CC = gcc
CFLAGS = -ansi -Wall -pedantic -I./headers

# Dependency header files
GLOBAL_DEPS = ./headers/globals.h ./headers/vars.h

# Object files needed to create the executable
EXE_DEPS = hashTable.o preAssembler.o utils.o extTable.o symbolTable.o dataHandlers.o cmdHandlers.o first_pass.o extTable.o second_pass.o writeFiles.o assembler.o 
# Executable name
TARGET = asm

# Default target
all: $(TARGET)

# Linking all object files to create the executable
$(TARGET): $(EXE_DEPS)
	$(CC) $(CFLAGS) -g -o $@ $^

# Compiling individual source files into object files

hashTable.o: hashTable.c ./headers/hashTable.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

preAssembler.o: preAssembler.c ./headers/preAssembler.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

first_pass.o: first_pass.c ./headers/first_pass.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

second_pass.o: second_pass.c ./headers/second_pass.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

extTable.o: extTable.c ./headers/extTable.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

symbolTable.o: symbolTable.c ./headers/symbolTable.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

dataHandlers.o: dataHandlers.c ./headers/dataHandlers.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

cmdHandlers.o: cmdHandlers.c ./headers/cmdHandlers.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

utils.o: utils.c ./headers/utils.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

writeFiles.o: writeFiles.c ./headers/writeFiles.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@




# Cleaning up the object files and the executable
clean:
	rm -rf $(EXE_DEPS) $(TARGET)
