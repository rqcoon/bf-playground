typedef enum {
    Move,
    Change,
    Loop,
    Put,
    Get,
    Loop
} InstructionType;

typedef struct Instruction Instruction;

typedef struct {
    Instruction *items;
    size_t count;
    size_t capacity;
    int8_t error;
} InstructionList;

struct Instruction {
    InstructionType type;

    union {
        int offset;
        InstructionList loop;
    } data;
};

typedef struct {
    InstructionList instructions;
} Program;

typedef struct {
    Lexer *lexer;
    Token current;
    size_t token_idx;
    const char *error;
} Parser;

Program parse(Lexer *lexer);