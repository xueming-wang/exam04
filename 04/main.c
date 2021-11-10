#include <libc.h>
#include <stdio.h>
int main()
{
    char *a = "cd";
    char *b = "cd 12";
    int i = strcmp(a, b);
    printf("%d", i);
    return 0;
}
