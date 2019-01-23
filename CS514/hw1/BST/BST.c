#include <stdio.h>
#include <stdlib.h>

struct treenode {
   struct treenode* left;
   struct treenode* right;
   int value;
};


//insert value targ into BST
void insert(struct treenode *root,int val)
{
  if(val > root->value){
    //if greater, add to right
    if(root->right == NULL){
      //if there is nothing there, set it to leaf
      struct treenode* leaf=(struct treenode*)malloc(sizeof(struct treenode));
      leaf->value = val;
      leaf->left = NULL;
      leaf->right = NULL;
      root->right = leaf;
    }
    else{
      //if there is something there, recurse
      insert(root->right, val);
    }
  }
  else{
    //if equal to or less than, add to left
    if(root->left == NULL){
      //if there is nothing there, set it to leaf
      struct treenode* leaf=(struct treenode*)malloc(sizeof(struct treenode));
      leaf->value = val;
      leaf->left = NULL;
      leaf->right = NULL;
      root->left = leaf;
    }
    else{
      //if there is something there, recurse
      insert(root->left, val);
    }
  }

}

void preorderPrint(struct treenode* head){
  if(head != NULL)
  {
    printf("%d\n", head->value);
    preorderPrint(head->left);
    preorderPrint(head->right);
  }
}



//do it again, without recursion!?
void nr_preorderPrint(struct treenode* head){
  if(head != NULL)
  {
    printf("%d\n", head->value);
    preorderPrint(head->left);
    preorderPrint(head->right);
  }
}

int main(int argc, char **argv){
  struct treenode* head;
  head=(struct treenode*)malloc(sizeof(struct treenode));
  head->value = 10;
  head->left = NULL;
  head->right = NULL;

  insert(head, 20);
  insert(head, 8);
  insert(head, 9);
  insert(head, 2);
  insert(head, 25);

  preorderPrint(head);
}
