#include <stdlib.h>
#include <stdbool.h>

#include "parser.h"
#include "lexer.h"

void advance(Parser *parser) {
    parser->current = next_token(parser->lexer);
    if (parser->current.type != TokenEOF) {
        parser->token_idx++;
    }
}

bool count_tokens(Lexer *lexer, size_t *count) {
    Lexer scan = *lexer;
    *count = 0;

    while (1) {
        Token token = next_token(&scan);
        if (token.type == TokenEOF) {
            return true;
        }

        if (*count == SIZE_MAX) {
            return false;
        }

        (*count)++;
    }
}

bool append_instruction(InstructionList *list, Instruction instruction) {
    if (list->count >= list->capacity) {
        return false;
    }

    list->items[list->count++] = instruction;
    return true;
}

bool append_op(InstructionList *list, InstructionType type, int32_t offset) {
    if (list->count > 0) {
        Instruction *previous = &list->items[list->count - 1];
        if (previous->type == type) {
            if ((offset > 0 && previous->data.offset > INT32_MAX - offset) ||
                (offset < 0 && previous->data.offset < INT32_MIN - offset)) {
                    return false;
                }
            previous->data.offset += offset;

            if (previous->data.offset == 0) {
                list->count--;
            }

            return true;
        }
    }

    return append_instruction(list, (Instruction) {
        .type = type,
        .data.offset = offset
    });
}

InstructionList parse_instructions(Parser *parser, bool in, size_t capacity) {
    // I'm not overly happy with using list.error as my way of erroring out
    // However I cannot think of anything better for now.
    // TODO: modify error handling to be more legible.
    InstructionList list = {0};
    list.error = 0;

    if (capacity > 0) {
        list.items = calloc(capacity, sizeof(Instruction));
        if (list.items == NULL) {
            parser->error = "Memory allocation failed";
            list.error = 1;
            return list;
        }

        list.capacity = capacity;

        while (parser->current.type != TokenEOF) {
            Token token = parser->current;
            advance(parser);

            switch (token.type) {
                case MoveRight:
                    if (!append_op(&list, Move, 1)) {
                        list.error = 1;
                        return list;
                    }
                    break;
                case MoveLeft:
                    if (!append_op(&list, Move, -1)) {
                        list.error = 1;
                        return list;
                    }
                    break;
                case Increment:
                    if (!append_op(&list, Change, 1)) {
                        list.error = 1;
                        return list;
                    }
                    break;
                case Decrement:
                    if (!append_op(&list, Change, -1)) {
                        list.error = 1;
                        return list;
                    }
                    break;
                case Output:
                    if (!append_instruction(&list, (Instruction){.type = Put})) {
                        list.error = 1;
                        return list;
                    }
                    break;
                case Input:
                    if (!append_instruction(&list, (Instruction){.type = Get})) {
                        list.error = 1;
                        return list;
                    }
                    break;
                case LoopStart:
                    InstructionList nested = parse_instructions(parser, true, capacity);
                    if (parser->error != NULL) {
                        list.error = 1;
                        return list;
                    }

                    if (nested.error != 0) {
                        list.error = nested.error;
                        return list;
                    }

                    if (!append_instruction(&list, 
                        (Instruction){
                            .type = Loop,
                            .data.loop = nested
                        }
                    )) {
                            list.error = 1;
                            return list;
                    }

                    break;
                
                case LoopEnd:
                    if (!in) {
                        parser->error = "Unexpected closing bracket";
                        list.error = 1;
                        return list;
                    }

                    return list;
                
                default:
                    parser->error = "Unexpected token";
                    list.error = 1;
                    return list;
            }
        }

        if (in) {
            parser->error = "Unclosed loop";
            list.error = 1;
            return list;
        }

        return list;
    }
}

// realistically this should be atomized into building the lexer, and then parsing
// for better error handling.
Program parse(Lexer *lexer) {
    size_t token_count = 0;
    if (!count_tokens(lexer, &token_count)) {
        // I need a bailout of sorts. This is fucking terrible.
        // todo: not this
        return (Program){NULL};
    }

    Parser parser = {
        .lexer = lexer
    };

    advance(&parser);

    Program program = (Program){
        .instructions = parse_instructions(&parser, false, token_count),
    };

    if (program.instructions.error != 0) {
        // todo: not this
        free_program(&program);
        return (Program){NULL};
    }

    return program;
}

void free_ilist(InstructionList *list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        if (list->items[i].type == Loop) {
            free_ilist(&list->items[i].data.loop);
        }
    }

    free(list->items);
    *list = (InstructionList){0};
}

void free_program(Program *p) {
    if (!p) {
        return;
    }

    free_ilist(&p->instructions);
}