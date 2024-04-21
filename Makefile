# Makefile for Assembler project

# Compiler settings
CC = gcc
CFLAGS = -ansi -Wall -pedantic

# Dependency header files
GLOBAL_DEPS = globals.h vars.h

# Object files needed to create the executable
EXE_DEPS = utils.o extTable.o symbolTable.o dataHandlers.o cmdHandlers.o first_pass.o extTable.o second_pass.o writeFiles.o assembler.o 
# Executable name
TARGET = asm

# Default target
all: $(TARGET)

# Linking all object files to create the executable
$(TARGET): $(EXE_DEPS)
	$(CC) $(CFLAGS) -g -o $@ $^

# Compiling individual source files into object files

assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

first_pass.o: first_pass.c first_pass.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

second_pass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

extTable.o: extTable.c extTable.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

symbolTable.o: symbolTable.c symbolTable.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

dataHandlers.o: dataHandlers.c dataHandlers.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

cmdHandlers.o: cmdHandlers.c cmdHandlers.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

writeFiles.o: writeFiles.c writeFiles.h $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@




# Cleaning up the object files and the executable
clean:
	rm -rf $(EXE_DEPS) $(TARGET)
