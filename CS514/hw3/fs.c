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

struct inode{
  int type; //file or dir
  int size;
  int inuse;
  int links;
  int block[100];
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


// read from file described by fd, starting at start and going to end.  returns it as a bit string
void * readRange(int fd, int start, int end){
  void * data = malloc(end-start);
  printf("size is %d\n", sizeof(data));
  if (lseek(fd, start, SEEK_SET) == -1){
    perror("seek failed");
    exit(EXIT_FAILURE);
  }
  else{
    read(fd, data, (end-start));
  }

  return data;
}


void unmapfs(){
  munmap(fs, FSSIZE);
}


void formatfs(int fd){
  printf("formatting FS\n");
  //installs superblock and bitmap in FS
  struct superblock *M=malloc(sizeof(struct superblock));
  M->numOfBlocks = 2000;
  M->numOfInodes = 50;
  M->sizeOfInodes = 100;


  if (lseek(fd, 0, SEEK_SET) == -1){
    perror("seek failed");
    exit(EXIT_FAILURE);
  }
  else{
    write(fd, M, sizeof(struct superblock));
    printf("wrote superblock\n");
  }
}

void meta(int fd){
  printf("printing FS\n");
  //installs superblock and bitmap in FS
  struct superblock *N=malloc(sizeof(struct superblock));
  int end = 16;



  N = readRange(fd, 4, end+4);
  printf("blocks:%d\ninodes:%d\nsize:%d\n", N->numOfBlocks, N->numOfInodes, N->sizeOfInodes);

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


void lsfs(){

}

void addfilefs(char* fname){

}


void removefilefs(char* fname){

}


void extractfilefs(char* fname){

}
