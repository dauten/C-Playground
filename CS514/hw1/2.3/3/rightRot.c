#include <stdio.h>

int rightRot(int x, int n){
  for(int i = 0; i < n; i++){
    if(x % 2 == 1){
      x = x >> 1;
      //and then make 16th bit from the right 1.
      x |= 1UL << 15;
    }
    else
      x = x >> 1;
  }
  return x;

}

int main(int argc, char* argv){
  printf("The number 60 bitrotated 4 to the right is %d\n", rightRot(60, 4));
  return 0;
}
