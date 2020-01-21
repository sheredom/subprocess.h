#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif // _MSC_VER


// This convoluted function returns 0
// but will hopefully not be optimized away in release builds...
int return_0_non_optimizable()
{
  char buffer[100];
  long value = 62831853;
  sprintf(buffer, "%ld", value);
  char *c = buffer;
  int result = 0;
  while (*c) {
    int digit = (int)(c[0] - '0');
    result = result + digit;
    c++;
  }
  return result - 36;
}

int main()
{
  int p = 42;
  int q = return_0_non_optimizable();
  int r = p / q; // this is an integer division by zero
  printf("r=%d\n", r);
  return 0;
}
