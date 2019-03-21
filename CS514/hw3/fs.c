#include "fs.h"

void mapfs(int fd){
  if ((fs = mmap(NULL, FSSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL){
      perror("mmap failed");
      exit(EXIT_FAILURE);
  }
}


void unmapfs(){
  munmap(fs, FSSIZE);
}


void formatfs(){

}


void loadfs(){
  //this is going to get this needed information,
  //# of blocks available, # of inodes, size of inodes,
  //and a bitmap of length==# of blocks correspondng to used blocks
  //it should get and print this info
}


void lsfs(){

}

void addfilefs(char* fname){

}


void removefilefs(char* fname){

}


void extractfilefs(char* fname){

}

/*
* Adds superblocks and other meta-structure to file
*/
void init(char* fname){
  write(fd, "\0", 1) == -1)
}
