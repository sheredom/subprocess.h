#include <stdio.h>
#include <math.h>

int main()
{
    int p = (int)cos(0.);
    int q = (int)sin(0.);
    int r = p / q; // this is an integer division by zero
    printf("%d", r);
    return 0;
}
