#include "lexer.h"

char peek(Lexer *lexer) {
    if (lexer->cursor >= lexer->length) {
        return '\0';
    }

    return lexer->src[lexer->cursor];
}

char advance(Lexer *lexer) {
    if (lexer->cursor >= lexer->length) {
        return '\0';
    }

    return lexer->src[lexer->cursor++];
}

Token next_token(Lexer *lexer) {
    while (1) {
        char c = peek(lexer);

        switch (c) {
            case '>':
                advance(lexer);
                return (Token){MoveRight, '>'};

            case '<':
                advance(lexer);
                return (Token){MoveLeft, '<'};

            case '+':
                advance(lexer);
                return (Token){Increment, '+'};

            case '-':
                advance(lexer);
                return (Token){Decrement, '-'};

            case '.':
                advance(lexer);
                return (Token){Output, '.'};

            case ',':
                advance(lexer);
                return (Token){Input, ','};

            case '[':
                advance(lexer);
                return (Token){LoopStart, '['};

            case ']':
                advance(lexer);
                return (Token){LoopEnd, ']'};

            case '\0':
                return (Token){TokenEOF, '\0'};

            default:
                advance(lexer);
                break;
        }
    }
}