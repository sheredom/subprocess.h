
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This convoluted function returns 0
// but will hopefuly not be optimzed away in release builds...
int return_0_non_optimizable()
{
  char buffer[100];
  long value = 62831853071796;
  snprintf(buffer, 100, "%ld", value);
  char *c = buffer;
  int result = 0;
  while (*c) {
    result = result + *c - '\0';
    c++;
  }
  return result - 738;
}

int main()
{
  int p = 42;
  int q = return_0_non_optimizable();
  int r = p / q; // this is an integer division by zero
  printf("r=%d\n", r);
  return 0;
}
