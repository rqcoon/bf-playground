typedef enum {
    MoveRight,
    MoveLeft,
    Increment,
    Decrement,
    Input,
    Output,
    LoopStart,
    LoopEnd,
    TokenEOF
} TokenType;

typedef struct {
    TokenType type;
    char value;
} Token;

typedef struct {
    const char *src;
    size_t cursor;
    size_t length;
} Lexer;

Token next_token(Lexer *lexer);