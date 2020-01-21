#include <stdio.h>
 
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


// this function is infinitely recursive and will cause a stack overflow
int fun(int x) { 
  if (x == 1) 
      return 5;
  x = 6; 
  if (return_0_non_optimizable() == 0)
    fun(x);
  return x;
} 

int main(){
  int x = 5;
  int y;
  y = fun(x);
  printf("%d", y);
  return 0;
}
