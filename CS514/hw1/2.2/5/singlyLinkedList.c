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

void printList(struct listnode* head){
  printf("%d", head->value);
  while(head->next){
    head = head->next;
    printf("->%d", head->value);
  }
  printf("\n");
}

int getDepth(struct listnode* head){
  int depth = 0;
  while(head->next){
    head = head->next;
    depth++;
  }
  return depth;
}

struct listnode* findNthToLastElement(struct listnode* head, int N){

  return findNthElement(head, getDepth(head)-N );
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
  removeNthElement(&A, 3);
  printf("3rd Item (counting very 1st element as element zero)  removed\n");
  printList(&A);
}
