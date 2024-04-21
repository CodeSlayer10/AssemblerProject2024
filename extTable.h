
typedef struct external {
    char *name;
    int address;
    struct external *next;
} external;



void add_ext(external **head, char *name, int address);

void reset_ext(external **head);

