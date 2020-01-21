#include <stdio.h>
 
// this function is infinitely recursive and will cause a stack overflow
int fun(int x) { 
  if (x == 1) 
      return 5;
  x = 6; 
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
