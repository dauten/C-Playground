#include <stdio.h>


void printBinary(unsigned x){

}
int invert(unsigned x, int p, int n){
  //make an unsigned int t with same length of x and all bits from p to n
  //equal to 1, and all other bits as 0.  Then return XOR of x and t

  //we will construct our t by setting the rightmost n bits to 1 Then
  //rotating left length-(p+n)
  unsigned t = 0;
  for(int i = 0; i < n; i++){
    t++;
    t = t << 1;
  }

  t = t << (19-(p+n));

  return x^t;
}

int main(int argc, char* argv){
  printf("The number 64170 inverted from 7 to 12 is %d\n", invert(64170, 7, 5));
  return 0;
}
