# Makefile for Assembler project

# Compiler settings
CC = gcc
CFLAGS = -ansi -Wall -pedantic -I./headers

# Dependency header files
GLOBAL_DEPS = ./headers/globals.h ./headers/vars.h

# Object files needed to create the executable
EXE_DEPS = hashTable.o preAssembler.o utils.o extTable.o symbolTable.o dataHandlers.o cmdHandlers.o firstPass.o extTable.o secondPass.o writeFiles.o assembler.o 
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

firstPass.o: firstPass.c ./headers/firstPass.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

secondPass.o: secondPass.c ./headers/secondPass.h $(GLOBAL_DEPS)
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
