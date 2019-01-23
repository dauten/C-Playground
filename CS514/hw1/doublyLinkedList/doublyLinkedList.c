#include <stdio.h>

struct listnode {
   struct listnode* next;
   struct listnode* prev;
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

struct listnode* reverse(struct listnode* head){
  int d = getDepth(head); //get length of LL
  if(d % 2 == 0){ //list is even
    for(int i = 0; i < d/2; i++){
      struct listnode* temp;
      temp->next = head->next;
      temp->prev = head->prev;
      temp->value = head->value;
      head = findNthToLastElement(head, i);
    }
  }
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

  A.prev = 0;
  B.prev = &A;
  C.prev = &B;
  D.prev = &C;
  E.prev = &D;


  printList(&A);
  printf("3rd element is %d\n", findNthElement(&A, 0)->value);
  removeNthElement(&A, 3);
  printf("item removed\n");
  printList(&A);
  printf("0nd to last element is %d\n", findNthToLastElement(&A, 4)->value);

}
