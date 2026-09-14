#ifndef CHECK_H
#define CHECK_H

#include "lexic.h"

int check_tokens(lexer_t* lexer);
lex_status_t validate_preprocessor(lexer_t* lexer, int index);
lex_status_t validate_numbers(lexer_t* lexer, int index);
void check_brackets(lexer_t *lexer, int *error_found);
void check_missing_semicolon(lexer_t *lexer, int *error_found);
int is_control_keyword(const char *token);

#endif