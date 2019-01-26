#include <stdio.h>

int setBits(int x, int p, int n, int y){
  int t = 0;
  for(int i = 0; i < n; i++){
    t = t << 1;
    t++;

  }
  printf("t is %d\n", t);
  y = y&t;
  printf("y is %d\n", y);
  y = y << 12-p;
  printf("y is %d\n", y);

  x = x & ~(t << (12-p));
  printf("x is %d\n", x);
  return y | x;
}

int main(int argc, char* argv){
  printf("The number 43690 but setting bits 2 through 6 to the 4 rightmost bits of 52428 is %d\n", setBits(43690, 2, 4, 52428));

  return 0;
}
