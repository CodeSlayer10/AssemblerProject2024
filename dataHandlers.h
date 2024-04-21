#include "globals.h"

instruction find_instruction(char *line, int *index);
int find_label(char *line, char *symbol);
int defineHandler(char *arg);
int stringHandler(char *args);
int dataHandler(char *args);
int externHandler(char *arg);
int entryHandler(char *arg);
int entryValidator(char *arg);
