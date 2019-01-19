#include <stdio.h>

void set(int* a, int val)
{
  *a = val;
}

int main(int argc, char **argv){

  int a = 0;
  printf("a is %d\n", a);
  set(&a, 35);
  printf("a is %d\n", a);
  return 0;
}
