#ifndef LEXICAL_H
#define LEXICAL_H

#include <stdio.h>

#define MAX_TOKENS    1000
#define MAX_TOKEN_LEN 64

// colour codes
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define BOLD    "\x1b[1m"

// Status Codes 
typedef enum
{
    LEX_SUCCESS = 0,
    LEX_FAILURE
}lex_status_t;

// Token types
typedef enum
{
    TOKEN_PREPROCESSOR,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,

    TOKEN_NUMBER_DECIMAL,
    TOKEN_NUMBER_FLOAT,
    TOKEN_NUMBER_BINARY,
    TOKEN_NUMBER_OCTAL,
    TOKEN_NUMBER_HEXA,
    
    TOKEN_STRING,
    TOKEN_CHARACTER,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATOR,
    TOKEN_UNTERMINATED_STRING, 
    TOKEN_UNTERMINATED_CHAR,
    TOKEN_UNKNOWN
}token_type_t;

// Token structure
typedef struct
{
    token_type_t type;             // Token category
    char value[MAX_TOKEN_LEN];     // Lexeme
    int line_no;                   // Line number
    int col_no;
}token_t;

// Lexer Context
typedef struct 
{
    FILE* fp;                      // Source file pointer
    const char* filename;
    int current_line;              // Line counter
    int current_col;
    token_t tokens[MAX_TOKENS];    // Token list
    int token_count;               // Number of tokens        
}lexer_t;

// Lexer Tokenization - Phase - 1 

// Initialiasing lexer and open file
lex_status_t lexer_init(lexer_t* lexer, const char* filename);

// Lexical analysis
lex_status_t lexer_tokenize(lexer_t* lexer);

// print tokens
void lexer_print_tokens(const lexer_t* lexer); 

// Helper functions
void add_token(lexer_t *lexer, token_type_t type, const char *value);
int is_keyword(const char* str);
int is_operator(const char* str);
int is_punctuator(char ch);

#endif