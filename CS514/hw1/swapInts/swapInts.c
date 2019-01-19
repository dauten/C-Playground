#include <stdio.h>

void swapInts(int* a, int* b)
{
  int t = *a;
  *a = *b;
  *b = t;
}

int main(int argc, char **argv){

  int a = 20;
  int b = 10;
  printf("a is %d and b is %d\n", a, b);
  swapInts(&a, &b);
  printf("a is %d and b is %d\n", a, b);
  return 0;
}
