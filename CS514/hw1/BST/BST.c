#include <stdio.h>

struct treenode {
   struct treenode* left;
   struct treenode* right;
   int value;
};


//insert value targ into BST
void insert(struct treenode* head, int targ){

  printf("Current is %d new is %d\n", head->value, targ);
  if(targ <= head->value){
    if(head->left){
      printf("\tHeading left and recursing\n");
      insert(head->left, targ);
    }
    else{
      printf("\tHeading left to add it\n");
      struct treenode leaf;
      leaf.value = targ;
      leaf.left = 0;
      leaf.right = 0;
      head->left = &leaf;
      printf("added to the left of %d the value %d\n", head->value, leaf.value);
    }
  }
  else{ //if targ > head->value
    if(head->right){
      printf("\tHeading right and recursing\n");
      insert(head->right, targ);
    }
    else{
      printf("\tHeading right to add it\n");
      struct treenode leaf;
      leaf.value = targ;
      leaf.left = 0;
      leaf.right = 0;
      head->right = &leaf;
      printf("added to the right of %d the value %d\n", head->value, leaf.value);
    }
  }
}

int getDepth(struct treenode* head){
  int depth = 0;
  while(head->left){
    head = head->left;
    depth++;
  }
  return depth;
}

void preorderPrint(struct treenode* head){
  if(head->value)
    printf("%d\n", head->value);
  else
    return;
  if(head->left)
    preorderPrint(head->left);
  if(head->right)
    preorderPrint(head->right);
}

int main(int argc, char **argv){
  struct treenode head;
  head.value = 19;
  head.left = 0;
  head.right = 0;

//  preorderPrint(&head);

  insert(&head, 10);
  insert(&head, 30);
  preorderPrint(&head);/*
  insert(&head, 5);
  insert(&head, 15);
  insert(&head, 20);
  insert(&head, 25);*/

//  preorderPrint(&head);
}
