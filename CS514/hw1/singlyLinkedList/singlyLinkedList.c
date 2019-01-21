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

void removeNthElement(struct listnode* head, int N)
{
  for(int i = 0; i < N-1; i++){
    head = head->next;
  }
  head->next = head->next->next;
}

void printList(struct listnode* head, int N){
  for(int i = 0; i < N; i++){
    printf("%d->", head->value);
    head = head->next;
  }
  printf("null\n");
}

void findNthToLastElement(struct listnode* head, int N){

}

int main(int argc, char **argv){

  struct listnode A, B, C, D, E;
  A.value = 0;
  B.value = 1;
  C.value = 2;
  D.value = 3;
  E.value = 4;

  A.next = &B;
  B.next = &C;
  C.next = &D;
  D.next = &E;


  printList(&A, 5);
  printf("Nth element is %d\n", findNthElement(&A, 3)->value);
  removeNthElement(&A, 3);
  printf("item removed\n");
  printList(&A, 4);
}
