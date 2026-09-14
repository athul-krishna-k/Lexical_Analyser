#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexic.h"

static const char* keywords[] = {"auto", "break", "case", "char", "continue", "default", "do", "double", "else", 
    "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", 
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", 
    "void", "volatile", "while", NULL };

static const char* operators[] = { ">>=", "<<=", "==", "!=", "<=", ">=", "++", "--", "&&", "||", "+=", "-=", "*=", "/=", "%=",
    ">>", "<<", "+", "-", "*", "/", "%", "=", "<", ">", "!", "&", "|", "^", "~", NULL };

static const char punctuators[] = "(){}[];,.:?";

static const char* token_type_names[] = { "PREPROCESSOR DIRECTIVE", "KEYWORD", "IDENTIFIER", "DECIMAL NUMBER", 
    "FLOATING-POINT NUMBER", "BINARY NUMBER", "OCTAL NUMBER", "HEXADECIMAL NUMBER", 
    "STRING", "CHARACTER", "OPERATOR", "PUNCTUATOR", "UNTERMINATED STRING", 
    "UNTERMINATED CHAR", "UNKNOWN" };

lex_status_t lexer_init(lexer_t* lexer, const char* filename)
{
    char* ext = strrchr(filename, '.');
    if(ext == NULL || strcmp(ext, ".c") != 0)
    {
        printf("%s: Invalid extension\n", filename);
        return LEX_FAILURE;
    }

    FILE* fp = fopen(filename, "r");
    if(fp == NULL)
    {
        printf("%s: File not existing\n", filename);
        return LEX_FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    if(ftell(fp) == 0)
    {
        printf("%s: File is empty\n", filename);
        fclose(fp);
        return LEX_FAILURE;
    }
    rewind(fp);

    lexer->fp = fp;
    lexer->filename = filename;
    lexer->current_line = 1;
    lexer->current_col = 1;
    lexer->token_count = 0;

    return LEX_SUCCESS;
}

lex_status_t lexer_tokenize(lexer_t* lexer)
{
    int ch;
    while((ch = fgetc(lexer->fp)) != EOF)
    {
        lexer->current_col++;

        if(ch == ' ' || ch == '\t')
            continue;
        else if(ch == '\r')
            continue;
        else if(ch == '\n')
        {
            lexer->current_line++;
            lexer->current_col = 1;
            continue;
        }
        // Preprocessor
        else if(ch == '#')
        {
            char buffer[MAX_TOKEN_LEN];
            int i = 0;

            int token_line = lexer->current_line;
            int token_col = lexer->current_col;

            buffer[i++] = ch;

            while((ch = fgetc(lexer->fp)) != EOF && ch != '\n')
            {
                lexer->current_col++;
                if(ch == '\r') continue;
                if(i < MAX_TOKEN_LEN - 1)
                    buffer[i++] = ch;
            }

            buffer[i] = '\0';

            lexer->current_line = token_line;
            lexer->current_col = token_col;
            add_token(lexer, TOKEN_PREPROCESSOR, buffer);

            if(ch == '\n')
            {
                lexer->current_line++;
                lexer->current_col = 1;
            }
        }
        // Keyword / identifier
        else if(isalpha(ch) || ch == '_')
        {
            char buffer[MAX_TOKEN_LEN];
            int i = 0;

            while(isalnum(ch) || ch == '_')
            {
                buffer[i++] = ch;
                ch = fgetc(lexer->fp);
                lexer->current_col++;
            }
            buffer[i] = '\0';
            ungetc(ch, lexer->fp);

            if(is_keyword(buffer))
                add_token(lexer, TOKEN_KEYWORD, buffer);
            else
                add_token(lexer, TOKEN_IDENTIFIER, buffer);
        }
        // Dot (float starting with .)
        else if(ch == '.')
        {
            int next = fgetc(lexer->fp);

            if(isdigit(next))
            {
                char buffer[MAX_TOKEN_LEN];
                int i = 0;

                buffer[i++] = ch;
                buffer[i++] = next;
                lexer->current_col++;

                next = fgetc(lexer->fp);
                lexer->current_col++;

                while(isdigit(next))
                {
                    buffer[i++] = next;
                    next = fgetc(lexer->fp);
                    lexer->current_col++;
                }

                buffer[i] = '\0';

                if(next != EOF)
                {
                    ungetc(next, lexer->fp);
                    lexer->current_col--;
                }

                add_token(lexer, TOKEN_NUMBER_FLOAT, buffer);
            }
            else
            {
                if(next != EOF)
                {
                    ungetc(next, lexer->fp);
                    lexer->current_col--;
                }
                char buffer[2] = {ch, '\0'};
                add_token(lexer, TOKEN_PUNCTUATOR, buffer);
            }
        }
        // Number System
        else if(isdigit(ch))
        {
            char buffer[MAX_TOKEN_LEN];
            int i = 0;
            buffer[i++] = ch;
            int next = fgetc(lexer->fp);
            lexer->current_col++;
            token_type_t num_type = TOKEN_NUMBER_DECIMAL;

            // Hexadecimal
            if(ch == '0' && (next == 'X' || next == 'x'))
            {
                buffer[i++] = next;
                next = fgetc(lexer->fp);
                lexer->current_col++;
                while(isalnum(next))
                {
                    buffer[i++] = next;
                    next = fgetc(lexer->fp);
                    lexer->current_col++;
                }
                num_type = TOKEN_NUMBER_HEXA;
            }
            // Binary
            else if(ch == '0' && (next == 'b' || next == 'B'))
            {
                buffer[i++] = next;
                next = fgetc(lexer->fp);
                lexer->current_col++;
                while(isalnum(next))
                {
                    buffer[i++] = next;
                    next = fgetc(lexer->fp);
                    lexer->current_col++;
                }
                num_type = TOKEN_NUMBER_BINARY;
            }
            // Octal
            else if(ch == '0' && isdigit(next))
            {
                while(isalnum(next))
                {
                    buffer[i++] = next;
                    next = fgetc(lexer->fp);
                    lexer->current_col++;
                }
                num_type = TOKEN_NUMBER_OCTAL;
            }
            // Float
            else if(isalnum(next) || next == '.')
            {
                while(isalnum(next) || next == '.')
                {
                    if(next == '.' || next == 'f' || next == 'F')
                        num_type = TOKEN_NUMBER_FLOAT;
                    buffer[i++] = next;
                    next = fgetc(lexer->fp);
                    lexer->current_col++;
                }
            }

            buffer[i] = '\0';
            ungetc(next, lexer->fp);
            add_token(lexer, num_type, buffer);
        }
        // String
        else if(ch == '"')
        {
            char buffer[MAX_TOKEN_LEN];
            int i = 0, terminated = 0;
            buffer[i++] = '"';

            while((ch = fgetc(lexer->fp)) != EOF)
            {
                lexer->current_col++;
                if(ch == '"')
                {
                    buffer[i++] = ch;
                    terminated = 1;
                    break;
                }
                if(ch == '\n')
                {
                    ungetc(ch, lexer->fp);
                    break;
                }
                buffer[i++] = ch;
            }

            buffer[i] = '\0';
            add_token(lexer, terminated ? TOKEN_STRING : TOKEN_UNTERMINATED_STRING, buffer);
        }
        // Character
        else if(ch == '\'')
        {
            char buffer[MAX_TOKEN_LEN];
            int i = 0, terminated = 0;

            buffer[i++] = ch;

            while((ch = fgetc(lexer->fp)) != EOF)
            {
                lexer->current_col++;
                if(ch == '\n')
                {
                    ungetc(ch, lexer->fp);
                    break;
                }
                buffer[i++] = ch;
                if(ch == '\'')
                {
                    terminated = 1;
                    break;
                }
            }

            buffer[i] = '\0';
            add_token(lexer, terminated ? TOKEN_CHARACTER : TOKEN_UNTERMINATED_CHAR, buffer);
        }
        // Comments and division
        else if(ch == '/')
        {
            int next = fgetc(lexer->fp);
            if(next == '/')
            {
                while((next = fgetc(lexer->fp)) != '\n' && next != EOF);
                lexer->current_line++;
            }
            else if(next == '*')
            {
                int prev = 0, curr;
                while((curr = fgetc(lexer->fp)) != EOF)
                {
                    lexer->current_col++;
                    if(curr == '\n')
                        lexer->current_line++;
                    if(prev == '*' && curr == '/')
                        break;
                    prev = curr;
                }
                if(curr == EOF)
                    printf("Lexical Error: Unterminated comment at line %d\n", lexer->current_line);
            }
            else
            {
                ungetc(next, lexer->fp);
                add_token(lexer, TOKEN_OPERATOR, "/");
            }
        }
        // Operators
        else if(strchr("+-*%=<>!&|^~", ch))
        {
            char buffer[4] = {0};
            buffer[0] = ch;

            int ch2 = fgetc(lexer->fp);
            lexer->current_col++;

            if(ch2 != EOF)
            {
                buffer[1] = ch2;
                int ch3 = fgetc(lexer->fp);
                lexer->current_col++;

                if(ch3 != EOF)
                {
                    buffer[2] = ch3;
                    if(is_operator(buffer))
                    {
                        add_token(lexer, TOKEN_OPERATOR, buffer);
                        continue;
                    }
                    ungetc(ch3, lexer->fp);
                    lexer->current_col--;
                    buffer[2] = '\0';
                }

                if(is_operator(buffer))
                {
                    add_token(lexer, TOKEN_OPERATOR, buffer);
                    continue;
                }

                ungetc(ch2, lexer->fp);
                lexer->current_col--;
                buffer[1] = '\0';
            }

            add_token(lexer, is_operator(buffer) ? TOKEN_OPERATOR : TOKEN_UNKNOWN, buffer);
        }
        // Punctuator
        else if(is_punctuator(ch))
        {
            char buffer[2] = {ch, '\0'};
            add_token(lexer, TOKEN_PUNCTUATOR, buffer);
        }
        // Unknown
        else
        {
            char buffer[2] = {ch, '\0'};
            add_token(lexer, TOKEN_UNKNOWN, buffer);
        }
    }
    return LEX_SUCCESS;
}

void lexer_print_tokens(const lexer_t* lexer)
{
    printf("+------+----------------------+--------------------------+\n");
    printf("| Line | Token                | Type                     |\n");
    printf("+------+----------------------+--------------------------+\n");

    for(int i = 0; i < lexer->token_count; i++)
    {
        printf("| %-4d | %-20.20s | %-24.24s |\n",
               lexer->tokens[i].line_no,
               lexer->tokens[i].value,
               token_type_names[lexer->tokens[i].type]);
    }

    printf("+------+----------------------+--------------------------+\n");
}

int is_keyword(const char* str)
{
    for(int i = 0; keywords[i] != NULL; i++)
    {
        if(strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

int is_operator(const char* str)
{
    for(int i = 0; operators[i] != NULL; i++)
    {
        if(strcmp(str, operators[i]) == 0) return 1;
    }
    return 0;
}

int is_punctuator(char ch)
{
    return strchr(punctuators, ch) != NULL;
}

void add_token(lexer_t *lexer, token_type_t type, const char *value)
{
    if(lexer->token_count >= MAX_TOKENS)
        return;

    int idx = lexer->token_count++;
    lexer->tokens[idx].type = type;
    strncpy(lexer->tokens[idx].value, value, MAX_TOKEN_LEN - 1);
    lexer->tokens[idx].value[MAX_TOKEN_LEN - 1] = '\0';
    lexer->tokens[idx].line_no = lexer->current_line;
    lexer->tokens[idx].col_no = lexer->current_col;
}