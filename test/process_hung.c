#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif // _MSC_VER

// This convoluted function returns 0
// but will hopefully not be optimized away in release builds...
int return_0_non_optimizable() {
  char buffer[100];
  long value = 62831853;
  char *c;
  int result;

  sprintf(buffer, "%ld", value);
  c = buffer;
  result = 0;
  while (*c) {
    int digit = (int)(c[0] - '0');
    result = result + digit;
    c++;
  }
  return result - 36;
}

// this process will be hung in an infinite loop
int main() {
  int r = 0;
  while (return_0_non_optimizable() == 0)
    r = r + 1;
  printf("r=%d", r);
  return 0;
}