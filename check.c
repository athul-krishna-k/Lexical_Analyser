#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "check.h"
#include "lexic.h"

int check_tokens(lexer_t* lexer)
{
    int error_found = 0;

    for(int i = 0; i < lexer -> token_count; i++)
    {
        int line = lexer -> tokens[i].line_no;

        switch(lexer -> tokens[i].type)
        { 
            case TOKEN_PREPROCESSOR:
                if(validate_preprocessor(lexer, i) == LEX_FAILURE)
                    error_found = 1;
                break;

            case TOKEN_NUMBER_DECIMAL:
            case TOKEN_NUMBER_FLOAT:
            case TOKEN_NUMBER_BINARY:
            case TOKEN_NUMBER_OCTAL:
            case TOKEN_NUMBER_HEXA:
                if(validate_numbers(lexer, i) == LEX_FAILURE)
                    error_found = 1;
                break;
            
            case TOKEN_UNTERMINATED_STRING:
               printf("%s:%d:%d: " RED "error: " RESET "missing terminating \" character\n", lexer->filename, line, lexer->tokens[i].col_no);
                break;
            
            case TOKEN_UNTERMINATED_CHAR:
                printf("%s:%d:%d: " RED "error: " RESET "missing terminating ' character\n", lexer->filename, line, lexer->tokens[i].col_no);
                break;

            case TOKEN_UNKNOWN:
                printf("%s:%d:%d: " RED "error: " RESET "unknown token '%s'\n", lexer->filename, line, lexer->tokens[i].col_no, lexer->tokens[i].value);  
                error_found = 1;
                break;
            
            default:
                break;
        }
    }

    check_brackets(lexer, &error_found);
    check_missing_semicolon(lexer, &error_found);

    return error_found;
}
      
lex_status_t validate_preprocessor(lexer_t* lexer, int index)
{
    char* directives[] = {"include", "define", "undef", "ifdef", "ifndef", "endif", "if", "elif", "else", "pragma", NULL};
    char *line = lexer -> tokens[index].value;

    int i = 0, j = 0;
    char buffer[32];

    if(line[i++] != '#')
        return LEX_FAILURE;

    while(line[i] == ' ' || line[i] == '\t')
        i++;

    if(line[i] == '\0')
    {
        printf("%s:%d:%d: " RED "error: " RESET "missing preprocessor directive name\n", lexer->filename, lexer->tokens[index].line_no, lexer->tokens[index].col_no);
        return LEX_FAILURE;
    }

    while(isalpha(line[i]))
        buffer[j++] = line[i++];
    
    buffer[j] = '\0';

    for(int k = 0; directives[k] != NULL; k++)
    {
        if(strcmp(directives[k], buffer) == 0)
            return LEX_SUCCESS; 
    }

    printf("%s:%d:%d: " RED "error: " RESET "invalid preprocessor directive '#%s'\n",lexer->filename,lexer->tokens[index].line_no,lexer->tokens[index].col_no,buffer);

    return LEX_FAILURE;
}

lex_status_t validate_numbers(lexer_t* lexer, int index)
{
    int type = lexer->tokens[index].type;
    char* value = lexer->tokens[index].value;
    int line = lexer->tokens[index].line_no;
    int i = 0;
    
    if(type == TOKEN_NUMBER_DECIMAL)
    {
        while(value[i] != '\0')
        {
            if(!isdigit(value[i++]))
            {
               printf("%s:%d:%d: " RED "error: " RESET "invalid decimal constant '%s'\n",lexer->filename,lexer->tokens[index].line_no,lexer->tokens[index].col_no,value);
            }
        }
    }
    else if(type == TOKEN_NUMBER_BINARY)
    {
        if(value[2] == '\0')
        {
           printf("%s:%d:%d: " RED "error: " RESET "invalid binary digit '%c'\n", lexer->filename, line, lexer->tokens[index].col_no, value[i]);
        }

        for(i = 2; value[i] != '\0'; i++) 
        {
            if(value[i] != '0' && value[i] != '1') 
            {
                printf("%s:%d:%d: " RED "error: " RESET "invalid binary digit '%c'\n",lexer->filename,line,lexer->tokens[index].col_no, value[i]);
                return LEX_FAILURE;
            }
        }
    }
    else if(type == TOKEN_NUMBER_OCTAL) 
    {
        for(i = 1; value[i] != '\0'; i++) 
        {
            if(value[i] < '0' || value[i] > '7') 
            {
               printf("%s:%d:%d: " RED "error: " RESET "invalid octal digit '%c'\n",lexer->filename,line,lexer->tokens[index].col_no, value[i]);
                return LEX_FAILURE;
            }
        }
    }
    else if(type == TOKEN_NUMBER_HEXA) 
    {
        if(value[2] == '\0') 
        { 
           printf("%s:%d:%d: " RED "error: " RESET "invalid hex digit '%c'\n",lexer->filename,line,lexer->tokens[index].col_no,value[i]);
            return LEX_FAILURE; 
        }

        for(i = 2; value[i] != '\0'; i++) 
        {
            if(!isxdigit(value[i])) 
            {
               printf("%s:%d:%d: " RED "error: " RESET "invalid hex digit '%c'\n",lexer->filename,line,lexer->tokens[index].col_no,value[i]);
            }
        }
    }
    else if(type == TOKEN_NUMBER_FLOAT) 
    {
        int dot_count = 0, suffix_count = 0, len = strlen(value);

        for(i = 0; i < len; i++) 
        {
            if(value[i] == '.') 
                dot_count++;
            else if(isalpha(value[i])) 
            {
                if(strchr("fFlL", value[i])) 
                {
                    suffix_count++;
                    if(i != len - 1) 
                    { 
                        printf("%s:%d:%d: " RED "error: " RESET "invalid suffix '%s'\n",lexer->filename,line,lexer->tokens[index].col_no,value);
                        return LEX_FAILURE; 
                    }
                } 
                else 
                {
                   printf("%s:%d:%d: " RED "error: " RESET "invalid suffix '%s'\n",lexer->filename, line, lexer->tokens[index].col_no,value);
                    return LEX_FAILURE;
                }
            }
        }
        if(dot_count > 1 || suffix_count > 1) 
        {
            printf("%s:%d:%d: " RED "error: " RESET "invalid float '%s'\n",lexer->filename,line,lexer->tokens[index].col_no, value);
            return LEX_FAILURE;
        }
    }
    return LEX_SUCCESS;
}

void check_brackets(lexer_t *lexer, int *error_found)
{
    char stack[100];
    int top = -1;

    for(int i = 0; i < lexer->token_count; i++)
    {
        if(lexer->tokens[i].type != TOKEN_PUNCTUATOR)
            continue;

        char ch = lexer->tokens[i].value[0];
        int line = lexer->tokens[i].line_no;

        if(ch == '(' || ch == '{' || ch == '[')
        {
            stack[++top] = ch;
        }
        else if(ch == ')' || ch == '}' || ch == ']')
        {
            if(top < 0)
            {
                printf("%s:%d:%d: " RED "error: " RESET "extra '%c'\n", lexer->filename, line, lexer->tokens[i].col_no, ch);
                *error_found = 1;
            }
            else
            {
                char open = stack[top--];
                if((open == '(' && ch != ')') || (open == '{' && ch != '}') || (open == '[' && ch != ']'))
                {
                    printf("%s:%d:%d: " RED "error: " RESET "mismatched '%c' and '%c'\n", lexer->filename, line, lexer->tokens[i].col_no, open, ch);
                    *error_found = 1;
                }
            }
        }
    }

    if(top >= 0)
    {
        printf("%s:%d:%d: " RED "error: " RESET "unclosed bracket '%c'\n",lexer->filename,lexer->tokens[lexer->token_count - 1].line_no,lexer->tokens[lexer->token_count - 1].col_no, stack[top]);
        *error_found = 1;
    }
}

void check_missing_semicolon(lexer_t *lexer, int* error_found)
{
    if (lexer->token_count == 0) 
        return;
    
    int current_line = lexer->tokens[0].line_no;
    char last_token[MAX_TOKEN_LEN] = "";     
    token_type_t last_type = TOKEN_UNKNOWN;
    int skip_line = 0;
    
    for(int i = 0; i <= lexer->token_count; i++)
    {
        if(i < lexer->token_count && lexer->tokens[i].line_no != current_line || i == lexer->token_count)
        {
            if(!skip_line && last_token[0] != '\0' &&
               strcmp(last_token, ";") != 0 && strcmp(last_token, "{") != 0 &&
               strcmp(last_token, "}") != 0 && strcmp(last_token, ")") != 0 &&
               strcmp(last_token, ":") != 0 &&
               !is_control_keyword(last_token) &&
               last_type != TOKEN_OPERATOR)
            {
                printf("%s:%d:%d: " RED "error: " RESET "missing semicolon\n",lexer->filename,current_line,lexer->tokens[i-1].col_no);
                *error_found = 1;
            }

            if(i == lexer->token_count) 
                break;

            current_line = lexer->tokens[i].line_no;
            last_token[0] = '\0';
            skip_line = 0;
        }

        if(lexer->tokens[i].type == TOKEN_PREPROCESSOR || 
           lexer->tokens[i].type == TOKEN_UNKNOWN || 
           lexer->tokens[i].type == TOKEN_UNTERMINATED_STRING || 
           lexer->tokens[i].type == TOKEN_UNTERMINATED_CHAR) 
        {
            skip_line = 1;
        } 
        else 
        {
            strcpy(last_token, lexer->tokens[i].value);
            last_type = lexer->tokens[i].type;
        }
    }
}

int is_control_keyword(const char *token)
{
    return (strcmp(token, "if") == 0 || strcmp(token, "else") == 0 ||
            strcmp(token, "for") == 0 || strcmp(token, "while") == 0 ||
            strcmp(token, "switch") == 0 || strcmp(token, "do") == 0);
}