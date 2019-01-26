#include <stdio.h>

int bitcount(unsigned x)
{
  int t = 0;
  /*
  * works by checking the status of the rightmost bit and counting it.
  * then we rotate everything one to the right over and over checking
  * the right most bit each time.
  */
  for(int i = 0; i < sizeof(x) * 8; i++){
    if(x % 2 == 1)
      t++;
    x = x >> 1;
  }

  return t;

}

int main(int argc, char **argv){
  printf("The number 4090 has %d 1's\n", bitcount(4090));
  return 0;
}
