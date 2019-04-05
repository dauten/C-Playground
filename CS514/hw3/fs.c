#include "fs.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>

#include <errno.h>

#include <sys/mman.h>

/*
Find File(array path of format [A, B, C] to mean /A/B/C ):
    I = root inode (location known from superblock?)
    for i from 0 to len(array)-2:
      search I.contents for A's inode
        I = A.inode
    if I = inode of C:
      for every block in C.contents
*/


struct block{
  long numb;
  long size;
  int content[496];
};

struct inode{
  long numb;
  long type; //file or dir
  //if type = 1 (file) then size will be number of blocks this has and
  //if type = 2 (directory) then size will be number of items and content will be location of other inodes
  long size; //we dont need a long but its same type as stat.size returns
  long inuse;
  char name[100];
  short int content[100];
};

struct dir{
  char name[256]; //255 chars plus terminator
  int inodeNum;
};

struct superblock{
  int numOfBlocks;
  int numOfInodes;
  int sizeOfInodes;
};

struct bitmap{

};

void mapfs(int fd){
  if ((fs = mmap(NULL, FSSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL){
      perror("mmap failed");
      exit(EXIT_FAILURE);
  }
}

//print binary data file to stdout
void out(void * file, int length){
  int j;
  for(j = 0; j < length; ++j)
    printf("%02x", ((u_int8_t*) file)[j]);
}

//print binary data file to stdout
void dump(void * file, int length){
  int j;
  for(j = 0; j < length; ++j)
    printf("%c", ((char*) file)[j]);
}

//read an inode, printing content
void checkInode(struct inode * I){

  for(int i = 0; i < I->size; i++){
    printf("%d\n", I->content[i]);
  }
}

// read from file described by fd, starting at start and going to end.  returns it as a bit string
void * readRange(int fd, int start, int end){
  void * data = malloc(end-start);
  if (lseek(fd, start, SEEK_SET) == -1){
    perror("seek failed");
    exit(EXIT_FAILURE);
  }
  else{
    read(fd, data, (end-start));
  }

  return data;
}

//given arbitrary data write that to start and end locations
void writeRange(int fd, void * data, int start, int end){
  if (lseek(fd, start, SEEK_SET) == -1){
    perror("seek failed");
    exit(EXIT_FAILURE);
  }
  else{
    write(fd, data, end-start);
  }
}


void unmapfs(){
  munmap(fs, FSSIZE);
}


void formatfs(int fd){
  printf("formatting FS\n");
  //installs superblock and bitmap in FS
  struct superblock *M=malloc(sizeof(struct superblock));
  M->numOfBlocks = 10000;
  M->numOfInodes = 10000;
  M->sizeOfInodes = sizeof(struct inode);

  writeRange(fd, M, 0, 12);

  //make root directory
  int i_zero = sizeof(struct superblock) + M->numOfBlocks/8; // first/0th inode
  //search for first unfilled inode and put this there.
  struct inode * temp = malloc(sizeof(struct inode));
  temp->type = 2;
  temp->name[0] = 'r';
  temp->inuse = 1;
  writeRange(fd, temp, i_zero, i_zero + sizeof(struct inode));
}




void loadfs(){
  //this is going to get this needed information,
  //# of blocks available, # of inodes, size of inodes,
  //and a bitmap of length==# of blocks correspondng to used blocks
  //it should get and print this info

  //512 bytes per block so there will be around 2000 blocks per FS
  //first 8 bytes of FS is # of blocks available.
  //second 8 bytes is # of inodes
  //third 8 bytes is size of inodes
}



//given the free block list return the integer correspondng to the first free block
//and update FBL so that bit is no longer 0
int freeBlockSearch(void * FBL){
  int j;
  int r = 0;
  int length = 1250;

  //iterate through the FBL searching fot the first 0 bit.
  for(j = 0; j < length; ++j){
    //if a given byte is 0xFF then its all ones, skip to next byte
    if( 0xFF == ((int*) FBL)[j]);
    else{
      int backup = j;
      //else it has at least one 0
      j = ((int*) FBL)[j];

      //while the first byte is one, we count how many left shits it takes to find 0
      while( (j & 0x80 ) != 0 ){
        r++;
        j <<= 1;
      }

      ((int *)FBL)[backup] |= 0x80>>(r%8);

      //return number of bits from left
      return r;
    }
    //in for loop if a byte has no 0's we count up eight, that entire byte
    r += 8;
  }
  //failed state
  return -1;
}

//return int address of given block
int blockAddress(short int block, int fd){
  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int start = sizeof(struct superblock) + N->numOfBlocks/8; //beginning of inodes
  start += N->numOfInodes * N->sizeOfInodes;


  return start + block*512;

}

//creates in fd an empty directory with given path
//"mkdir' was taken :(
struct inode * makeDir(struct inode * parent, int fd, char name[]){

  //find and allocate an inode directory with name names
  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode
  int I = i_zero;
  //search for first unfilled inode and put this there.
  struct inode * temp = malloc(sizeof(struct inode));
  struct inode * next = malloc(sizeof(struct inode));
  next->numb = -1;
  strcpy(next->name, name);
  next->type = 2;

  do{
    temp = readRange(fd, i_zero, i_zero+sizeof(struct inode));
    next->numb++;

    if(temp->inuse == 0){
      //add that to parent.content
      parent->content[parent->size] = next->numb;
      next->inuse = 1;
      parent->size++;
      writeRange(fd, parent, I + sizeof(struct inode)*parent->numb, I + sizeof(struct inode)*(parent->numb+1));
      writeRange(fd, next, I + sizeof(struct inode)*next->numb, I + sizeof(struct inode)*(next->numb+1));

      return next;
    }
    i_zero += sizeof(struct inode);
  }
  while(temp->inuse == 1);



}


//given the inode for a file, reconstruct file and out() it
void readFile(struct inode * F, int fd){

  for(int i = 0; i < F->size; i++){

    int addr = blockAddress(F->content[i], fd);

    struct block * B = malloc(sizeof(struct block));
    B = readRange(fd, addr, addr+512);

    void * data = readRange(fd, addr+16, addr+B->size);

    dump(B->content, B->size);

  }
}

//given an array of directory names ending in filename, searches for that file's inode
struct inode * getInodeByNumber(short int numb, int fd){
  struct inode * I;


  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode
  //search for first unfilled inode and put this there.
  int i = -1;
  struct inode * temp = malloc(sizeof(struct inode));
  do{
    I = readRange(fd, i_zero, i_zero+sizeof(struct inode));
    i_zero+=sizeof(struct inode);
    i++;
  }
  while(I->numb != numb);

  if(I->inuse == 0){
    printf("That led to a not-in-use inode.  The file may have been deleted.\n");
    exit(1);
  }

  return I;
}


//given an inode
void printTree(struct inode * I, int fd, int depth){
  for(int i = 0; i <= depth; i++){
    printf("  ");
  }

  printf("%s", I->name);
  if(I->type == 2){
    printf("/");
  }
  printf("\n");

  for(int i = 0; i < I->size; i++){
    //gets every number in I->content and calls this function
    //recursively on its inode
    struct inode * temp = getInodeByNumber(I->content[i], fd);

    if(temp->type == 2)
      printTree(temp, fd, depth + 1);
    else{
      for(int i = 0; i <= depth+1; i++){
        printf("  ");
      }
      printf("%s\n", temp->name);
    }

  }
}



void lsfs(int fd){
  //get first inode and printTree it
  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode

  //search for first unfilled inode and put this there.
  struct inode * temp = malloc(sizeof(struct inode));
  temp = readRange(fd, i_zero, i_zero+sizeof(struct inode));  //get root inode

  printTree(temp, fd, 0);
}



//given a string pathname, converts it to array of string correspondng to component
//files and directories (ie '/root/home/dale/sample.txt' becomes
//['root', 'home', 'dale', 'sample.txt'])
//returns number of components, 4 in above case
int pathNameConvert(char * file, char * path[], int length){
  int i = 0;
  char * pch;
  length--;
  pch = strtok (file,"/");
  while (pch != NULL)
  {
    path[i] = pch;
    i++;
    pch = strtok (NULL, "/");
  }

  return i-1;
}

int pathLength(char * file){
  int len = strlen(file);
  int count = 1;
  for(int i = 0; i < len; i++){
    if(file[i] == '/')
      count++;
  }
  return count;
}

void * readFBL(int fd){
  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int temp = N->numOfBlocks/8;
  void * FBL = malloc(temp);
  FBL = readRange(fd, sizeof(struct superblock), sizeof(struct superblock)+temp);

  return FBL;
}

void updateFBL(void * FBL, int fd){
  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int temp = N->numOfBlocks/8;

  writeRange(fd, FBL, sizeof(struct superblock), sizeof(struct superblock)+temp);
}



void addInode(struct inode * I, char * path[], int fd, int len){

  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode

  //search for first unfilled inode and put this there.
  struct inode * temp = malloc(sizeof(struct inode));
  temp = readRange(fd, i_zero, i_zero+sizeof(struct inode));  //get root inode

  struct inode * temp2 = malloc(sizeof(struct inode));

  int curEl = 0;
  int exists = 0;
  do{

    exists = 0;
    //for every item in this directpry, check if that item is next part of path
    for(int i = 0; i < temp->size; i++){
      temp2 = getInodeByNumber(temp->content[i], fd);
      if(strcmp(temp2->name, path[curEl]) == 0){

        i = temp->size;
        exists = 1;
      }
    }

    if(!exists){
      //then no folder in that dir exists, make one
      temp2 = makeDir(temp, fd, path[curEl]);
    }


    curEl++;

    temp = temp2;
  }
  while(strcmp(temp->name, path[len-3]) != 0); //while inode is a directory



  //create file like we used to only update temp to add it's number to temp.content
  i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode
  do{
    temp = readRange(fd, i_zero, i_zero+sizeof(struct inode));
    I->numb++;
    if(temp->inuse == 0){
      writeRange(fd, I, i_zero, i_zero+sizeof(struct inode));
      printf("Wrote file %s to %d\n", I->name, I->numb);
      i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode

      //update temp2->content and write
      temp2->content[temp2->size] = I->numb;
      temp2->size++;
      printf("The file is added to parent %s\n", temp2->name);
      writeRange(fd, temp2, i_zero + sizeof(struct inode)*temp2->numb, i_zero + sizeof(struct inode)*(temp2->numb+1));

      return;
    }
    i_zero += sizeof(struct inode);
  }
  while(temp->inuse == 1);




}

//given a pathname, returns inode for that file, updating
//or creating any directories when necessary
struct inode * findInode(char * path[]){

}

void addfilefs(char* fname, int fd){
  int in;
  if ((in = open(fname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
    perror("open failed");
    exit(EXIT_FAILURE);
  }
  else{
    struct stat st;
    fstat(in, &st);
    long size = st.st_size;
    void * d;
    void * FBL = readFBL(fd);

    if(size > 49600){
      printf("oops the indirect block pointer hasn't been implemented and that file looks like its too big!\n");
      return;
    }
    else{
      int pNum = pathLength(fname);
      char * paths[pNum];
      pathNameConvert(fname, paths, pNum);
      struct inode * I = malloc(sizeof(struct inode));
      I->size = (size/496)+1; //this may be an issue when size%496==0
      //for(int i = 0; i < strlen(paths[pNum-1]); i++){
      //  I->name[i] = paths[pNum-1][i];
      //  printf("%s", I->name[i]);
      //}


      for(int s = 0; s < strlen(fname); s++){
        I->name[s] = paths[pNum-2][s];
      }




      I->numb = -1;
      I->inuse = 1;
      I->type = 1; //for file

      int r = size%496;
      int s;

      for(int i = 0; i < 1+(size/496); i++){
        if(i == (size/496)){
          s = r;
        }
        else{
          s = 496;
        }
        d = readRange(in, (i*496), s + (i)*496);  //read next block of 496 bytes
        struct block * B = malloc(sizeof(struct block));

        for(int b = 0; b < s; b++){
          B->content[b] = ((int *)d)[b];
        }
        B->size = s;
        B->numb = freeBlockSearch(FBL); //FBL needs to be instantiated first tho

        I->content[i] = B->numb;
        int start = blockAddress(B->numb, fd);

        writeRange(fd, B, start, start+512);
      }
      //write the inode and update parents
      addInode(I, paths, fd, pNum);

      //write the updated FBL
      updateFBL(FBL, fd);

    }
  }
}

//marks blocks of numbers in blocks as free
void freeBlock(void * FBL, short int blocks){
  int chunk = blocks / 8; //which byte # in FBL are we accessing?
  short int target = 1;  //what specific with in chunk are we reseting?
  for(int i = 7; i > (blocks % 8); i--)
    target *= 2;
  ((int *)FBL)[chunk] ^= target;
}

//clears content and marks as not in use
void deleteInode(struct inode * F, int fd){

  void * FBL = readFBL(fd);
  F->inuse = 0;
  F->name[0] = 0;
  for(int i = 0; i < F->size; i++){
    freeBlock(FBL, F->content[i]);
    F->content[i] = 0;

  }

  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode

  i_zero = i_zero + F->numb * sizeof(struct inode);

  writeRange(fd, F, i_zero, i_zero+sizeof(struct inode));
  updateFBL(FBL, fd);
}

//given an array of directory names ending in filename, searches for that file's inode
struct inode * getInode(char * path[], int fd, int len){

  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode

  //search for first unfilled inode and put this there.
  struct inode * temp = malloc(sizeof(struct inode));
  temp = readRange(fd, i_zero, i_zero+sizeof(struct inode));  //get root inode
  struct inode * temp2 = malloc(sizeof(struct inode));

  int curEl = 0;

  do{
    //for every item in this directpry, check if that item is next part of path
    for(int i = 0; i < temp->size; i++){
      temp2 = getInodeByNumber(temp->content[i], fd);
      if(strcmp(temp2->name, path[curEl]) == 0){
        temp = temp2;
        i = 99999;

      }
    }

    curEl++;

    temp = temp2;
  }
  while(temp->type != 1); //while inode is a directory

  return temp;
}

void removefilefs(char* fname, int fd){

  int pNum = pathLength(fname);
  char * paths[pNum];


  paths[0] = fname;
  pathNameConvert(fname, paths, pNum);
  struct inode * I = getInode(paths, fd, pNum);
  deleteInode(I, fd);
}


void extractfilefs(char* fname, int fd){
  int pNum = pathLength(fname);
  char * paths[pNum];

  pathNameConvert(fname, paths, pNum);
  struct inode * I = getInode(paths, fd, pNum);

  readFile(I, fd);
}



void meta(int fd){

  struct inode * I;

  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  int i_zero = sizeof(struct superblock) + N->numOfBlocks/8; // first/0th inode
  //search for first unfilled inode and put this there.
  int i = -1;
  struct inode * temp = malloc(sizeof(struct inode));
  do{
    I = readRange(fd, i_zero, i_zero+sizeof(struct inode));
    i_zero+=sizeof(struct inode);
    i++;
    //printf info about inodes
    if(I->type != 0)
      printf("Inode %d has type %d, usage %d, and size %d and name of %s\n", I->numb, I->type, I->inuse, I->size, I->name);
      //for(int p = 0; p < I->size; p++){
      //  printf("\t%d\n", I->content[p]);
      //}
  }
  while(i_zero < blockAddress(0, fd));
  printf("any remaining inodes have never been allocated and are empty.\n");
}
