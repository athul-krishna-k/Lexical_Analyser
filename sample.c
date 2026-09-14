#include <stdio.h>

#define MAX 100

int main()
{
    // Integer types
    int a = 10
    int b = 077;
    int c = 0x1A3FG;
    int d = 0b10121;

    // Floating-point
    float x = 0.5;
    float k = 5.2f;

    // Character and string
    char ch = 'A';
    char *str = "Hello World;

    // Operators
    a = a + 5;
    a = a * 3;
    a += 1;
    a++;

    // Relational
    if (a > 5 && a < MAX)
    {
        a--;
    }

    // For loop
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", i);
    }

    return 0
}