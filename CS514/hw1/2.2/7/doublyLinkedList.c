#include <stdio.h>
#include <stdlib.h>

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

struct listnode* reverse(struct listnode** head){
  int f = 1;
  while((*head)->next){
    printf("this is %d\n", (*head)->value);
    struct listnode* temp = (*head)->prev;
    (*head)->prev = (*head)->next;
    (*head)->next = temp;
    if(f){
      f = 0;
      (*head)->next = NULL;
    }

    (*head) = (*head)->prev;

  }
  (*head)->next = (*head)->prev;
  (*head)->prev = NULL;
  return (*head);
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
  printf("rewind:\n");
  printList(reverse(&A));

}
