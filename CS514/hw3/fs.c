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
  void * content;
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
    printf("%02x\n", ((u_int8_t*) file)[j]);
}

//read an inode, printing content
void checkInode(struct inode * I){
  printf("type of %d, size of %d, in use?  %d.  name of %s\n", I->type, I->size, I->inuse, I->name);
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
}

void meta(int fd){

  printf("entering debugging\n");
  //read and print superblock
  struct superblock *N=malloc(sizeof(struct superblock));
  N = readRange(fd, 0, sizeof(struct superblock));
  printf("%d blocks,  %d inodes,  inodes are %d bytes\n", N->numOfBlocks, N->numOfInodes, N->sizeOfInodes);
  //read and print free block list
  int temp = N->numOfBlocks/8;
  void * FBL = malloc(temp);
  FBL = readRange(fd, sizeof(struct superblock), sizeof(struct superblock)+temp);
  printf("FBL has length of %d bytes\nThe contents of those bytes are (with each bit corresponding to a block, 1=used 0=free):\n", temp);
//  out(FBL, temp);
  //read and print inodes
  int f = sizeof(struct superblock)+temp;
  int t = f + sizeof(struct inode);
  printf("First inode goes from %d to %d\n", f, t);
  t = f + (N->numOfInodes*N->sizeOfInodes);
  f = t - N->sizeOfInodes;
  printf("Last inode goes from %d to %d\n", f, t);
  struct block *B = malloc(sizeof(struct block));
  B = readRange(fd, t, t+512);
  printf("first block has %d bytes in it.  content of first block in hex is:\n", B->size);
  out(B->content, B->size);
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

//given the inode for a file, reconstruct file and out() it
void readFile(struct inode F){

}

//given the free block list return the integer correspondng to the first free block
//and update FBL so that bit is no longer 0
int freeBlockSearch(void * FBL){

  return -1;
}

//return int address of given block
int blockAddress(short int block, int fd){


  return -1;
}

//creates in fd an empty directory with given path
//"mkdir' was taken :(
void makeDir(int fd, char * path){

}

void lsfs(){

}

//given a string pathname, converts it to array of string correspondng to component
//files and directories (ie '/root/home/dale/sample.txt' becomes
//['root', 'home', 'dale', 'sample.txt'])
//returns number of components, 4 in above case
int pathNameConvert(char * file, char * path[]){

  return -1;
}

int pathLength(char * file){

  return -1;
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
    void * FBL;

    if(size > 49600){
      printf("oops the indirect block pointer hasn't been implemented and that file looks like its too big!\n");
      return;
    }
    else{
      int pNum = pathLength(fname);
      char * paths[pNum];
      pathNameConvert(fname, paths);
      struct inode * I = malloc(sizeof(struct inode));
      I->size = (size/496)+1; //this may be an issue when size%496==0
      for(int i = 0; i < pNum; i++){
        I->name[i] = paths[pNum-1][i];
      }

      for(int i = 0; i < (size/496); i++){
        d = readRange(in, (i*496), (i+1)*496);  //read next block of 496 bytes
        struct block * B = malloc(sizeof(struct block));
        B->content = d;
        B->size = 496;
        B->numb = freeBlockSearch(FBL);
      }

    }
  }
}


void removefilefs(char* fname){

}


void extractfilefs(char* fname, int fd){
    int in;
    void * d;
    d = readRange(fd, 100, 612);
    out(d, 512);
    if ((in = open(fname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
      perror("open failed");
      exit(EXIT_FAILURE);
    }
    else{
      write(in, d, 512);
    }
}
