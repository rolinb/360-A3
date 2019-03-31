#include <stdio.h>
#include <stdlib.h>
#include "../disk/disk.h"


/*
*Code taken from tutorial 9
*/


char* createEmptyInode(){
  char* inode = malloc(32);
  return inode;
}

void addFileSizeToInode(char* inode, int size){
  inode[0] = size;
}

void addAddressToInode(char* inode, int address){
  inode[8] = address; //first address this isn't good but its a start
}


int findNextAvailableBlock(FILE* disk){
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 1, buffer);
  int i;
  for (i=0; i<BLOCK_SIZE; i++){
    if(buffer[i] == 0b00000000){
      continue;
    }
    int j;
    char tmp = buffer[i];
    for(j=0; j<8; j++){
      if((tmp&0b10000000) == 0b10000000){
        return i*8+j;
      }
      tmp = tmp<<1;
    }
  }
  return -1;
}


int findNextAvailableBlockAndUseIt(FILE* disk){
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 1, buffer);
  int i;
  for (i=0; i<BLOCK_SIZE; i++){
    if(buffer[i] == 0b00000000){
      continue;
    }
    int j;
    char tmp = buffer[i];
    char unsigned useful = 0b10000000;
    for(j=0; j<8; j++){
      if((tmp&useful) == useful){
        useful = useful>>j;
        buffer[i] = buffer[i]^useful;
        writeBlock(disk, 1, buffer);
        free(buffer);
        return i*8+j;
      }
      tmp = tmp<<1;
    }
  }
  return -1;
}

char* createFile(FILE* disk) {
    char* inode = createEmptyInode();
    // Add more things to inode?
    writeBlock(disk, findNextAvailableBlockAndUseIt(disk), inode);

    return inode;
    free(inode);
}

int writeFile(FILE* disk, char* data){
  char* inode = createFile(disk);
  int blockToUse = findNextAvailableBlock(disk);
  addFileSizeToInode(inode, blockToUse);
  writeBlock(disk, findNextAvailableBlockAndUseIt(disk), data);
  return 0;
}



int main (int argc, char* argv[]){
  createDisk();

  //this is for super block
  FILE* disk = fopen("../disk/vdisk", "r+b");
  writeBlock(disk, 0, "ABBA40964096");

  char freeBlock[4096] = {0b00000000, 0b00111111, [2 ... 511] = 0b11111111 };
  writeBlock(disk, 1, freeBlock);

  int nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 10: %d\n", nextFreeBlock);
  writeFile(disk, "PLEASE WORK");
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 12 : %d\n", nextFreeBlock);

  writeFile(disk, "WHAT IF YOU ARE DIFFERENT");
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 14: %d\n", nextFreeBlock);

  writeFile(disk, "PLEASE WORK");
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 16 : %d\n", nextFreeBlock);

  writeFile(disk, "WHAT IF YOU ARE DIFFERENT");
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 18: %d\n", nextFreeBlock);

  writeFile(disk, "PLEASE WORK");
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 20 : %d\n", nextFreeBlock);

  writeFile(disk, "WHAT IF YOU ARE DIFFERENT");
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 22: %d\n", nextFreeBlock);





  fclose(disk);
  //this is for super block
  //FILE* disk = fopen("../disk/vdisk", "w+b");
  //writeBlock(disk, 1, "0000");
  //fclose(disk);







}
