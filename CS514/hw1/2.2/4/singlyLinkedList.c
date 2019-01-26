#include <stdio.h>

struct listnode {
   struct listnode* next;
   int value;
};

/*
* Index starts at 0, so zero for first element, 1 for second, etc
*/
struct listnode* findNthElement(struct listnode* head, int N)
{
  for(int i = 0; i < N; i++){
    head = head->next;
  }
  return head;
}


void printList(struct listnode* head){
  printf("%d", head->value);
  while(head->next){
    head = head->next;
    printf("->%d", head->value);
  }
  printf("\n");
}

int main(int argc, char **argv){

  struct listnode A, B, C, D, E;
  A.value = 10;
  B.value = 11;
  C.value = 21;
  D.value = 31;
  E.value = 41;

  A.next = &B;
  B.next = &C;
  C.next = &D;
  D.next = &E;
  E.next = 0;

  printf("The list is:\n");
  printList(&A);
  printf("3rd element is %d\n", findNthElement(&A, 3)->value);
}
