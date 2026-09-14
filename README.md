# Lexical Analyzer in C

A **Lexical Analyzer** implemented in C that reads a C source file, analyzes its contents and identifies different types of tokens such as keywords, identifiers, constants, operators, special symbols, and strings.

The project also performs lexical error checking for invalid numeric formats and unterminated strings/character literals.

## Features

* Reads and analyzes a C source file
* Identifies different types of tokens
* Recognizes:

  * Keywords
  * Identifiers
  * Integer constants
  * Floating-point constants
  * Character constants
  * String literals
  * Operators
  * Special symbols
* Supports different numeric formats:

  * Decimal
  * Octal
  * Hexadecimal
  * Binary
* Detects lexical errors such as:

  * Invalid octal numbers
  * Invalid hexadecimal numbers
  * Invalid binary numbers
  * Invalid floating-point numbers
  * Missing terminating quotes
* Displays token information with line and column positions

## Project Structure

```text
LEXICAL_ANALYSER/
│
├── main.c
├── lexic.c
├── lexic.h
├── check.c
├── check.h
└── sample.c
```

## How It Works

The lexical analyzer processes the input source file character by character.

The general flow is:

```text
C Source File
      ↓
Read Characters
      ↓
Identify Lexemes
      ↓
Generate Tokens
      ↓
Check Lexical Errors
      ↓
Display Tokens
```

## Token Categories

The analyzer identifies tokens such as:

| Token Type     | Example                        |
| -------------- | ------------------------------ |
| Keyword        | `int`, `if`, `while`, `return` |
| Identifier     | `main`, `count`, `value`       |
| Integer        | `10`, `25`                     |
| Float          | `3.14`                         |
| Character      | `'A'`                          |
| String         | `"Hello"`                      |
| Operator       | `+`, `-`, `*`, `/`, `==`       |
| Special Symbol | `(`, `)`, `{`, `}`, `;`        |

## Error Detection

The project checks for invalid lexical patterns, including:

```text
Invalid octal number
Invalid hexadecimal number
Invalid binary number
Invalid floating-point number
Missing terminating quote
```

For example:

```c
int value = 089;
```

can be reported as an invalid octal number.

Similarly:

```c
float value = 3.4.5;
```

can be reported as an invalid floating-point constant.

## Compilation

Compile the project using GCC:

```bash
gcc main.c lexic.c check.c -o lexer
```

For additional compiler warnings:

```bash
gcc -Wall -Wextra -o lexer main.c lexic.c check.c
```

## Execution

Run the lexical analyzer by providing a C source file as an argument:

```bash
./lexer sample.c
```

## Sample Input

```c
#include <stdio.h>

int main()
{
    int num = 10;
    float value = 3.14;

    if (num > 5)
    {
        printf("Hello World\n");
    }

    return 0;
}
```

## Technologies Used

* **Language:** C
* **Compiler:** GCC
* **Platform:** Linux / WSL
* **Concepts:** Lexical Analysis, File Handling, Strings, Structures, Pointers, Tokenization
