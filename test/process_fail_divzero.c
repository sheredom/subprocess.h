#include <stdio.h>

int main()
{
    int p = 42;
    int q = p - 2 * 21;
    int r = p / q; // this is an integer division by zero
    printf("%d", r);
    return 0;
}
