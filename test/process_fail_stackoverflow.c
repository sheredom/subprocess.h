#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif // _MSC_VER

// This convoluted function returns 0
// but will hopefully not be optimized away in release builds...

#ifdef __clang__
__attribute__((optnone))
#endif
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

// this function is infinitely recursive and will cause a stack overflow
#ifdef __clang__
__attribute__((optnone))
#endif
int fun(int x) {
  if (x == 1)
    return 5;
  x = 6;
  if (return_0_non_optimizable() == 0)
    fun(x);
  return x;
}

int main() {
  int x = 5;
  int y;

  y = fun(x);
  printf("%d", y);
  return 0;
}
