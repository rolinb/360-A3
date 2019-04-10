#include <stdio.h>
#include <stdlib.h>
#include "../disk/disk.h"

int currentInode =0;
int MAX_INODES = 650;
/*
*Code taken from tutorial 9
*/
#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i) //convert to hex digit


char* createEmptyInode(){
  char* inode = malloc(32);
  return inode;
}

char* findInode(FILE* disk){
  //WHAT DO I DO HERE
  return NULL;
}


/*
inode map
Takes blocks 2-9 stores address of each inodes location e.g
inode 1 - block x
inode 2 - block y
Inode num needs 3 bytes as does which block? 1.5 for each so if we say 4 its cleaner
lets use 1024 then we'll take 2 bytes for the inode num followed by 2 for the address
*/
void createInodeMap(FILE* disk){
  //int counter =0;
  int i;
  char hex[2];
  //unsigned char  tmp, tmp2;
  //set up map? in a janky fashion
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 2, buffer);
  for(i=0; i<128; i++){
        hex[0] = (i) >>8;
        hex[1] = (i) & 0xff;
        buffer[i*4] = hex[0];
        buffer[i*4+1] = hex[1];
  }
  writeBlock(disk, 2, buffer);
  free(buffer);
  buffer = malloc(sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 3, buffer);
  for(i=128; i<256; i++){
        hex[0] = (i) >>8;
        hex[1] = (i) & 0xff;
        buffer[i*4] = hex[0];
        buffer[i*4+1] = hex[1];
  }
  writeBlock(disk, 3, buffer);
  readBlock(disk, 4, buffer);
  for(i=256; i<384; i++){
        hex[0] = (i) >>8;
        hex[1] = (i) & 0xff;
        buffer[i*4] = hex[0];
        buffer[i*4+1] = hex[1];
  }
  writeBlock(disk, 4, buffer);



}
//mark the inode map find where and add the address
void updateInodeMap(FILE* disk, int where){

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

//shouldn't need this anymore
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

//Specifically made to mark the block it needs
int reserveBlock(int blockNum, FILE* disk){
  if (blockNum > NUM_BLOCKS) return -1; //exit if trying to get block too big
  int numBytes = blockNum/8;
  int whichBit = blockNum%8;
  int i;
  //printf("numBytes %d\n", numBytes);

  char* buffer = malloc (sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 1, buffer);
  char unsigned xorBuffer = 0b10000000;
  for(i=0; i<whichBit; i++)
    xorBuffer = xorBuffer>>1;

  buffer[numBytes] ^= xorBuffer;
  writeBlock(disk, 1, buffer);
  return 0;
}

int markBlockFree(int blockNum, FILE* disk){
  if(blockNum > NUM_BLOCKS) return -1;


  int numBytes = blockNum/8;
  int whichBit = blockNum%8;
  int i;
  //printf("numBytes %d\n", numBytes);

  char* buffer = malloc (sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 1, buffer);
  char unsigned orBuffer = 0b10000000;
  for(i=0; i<whichBit; i++)
    orBuffer = orBuffer>>1;

  buffer[numBytes] |= orBuffer;
  writeBlock(disk, 1, buffer);
  return 0;


}

char* createFile(FILE* disk) {
    char* inode = createEmptyInode();
    // Add more things to inode?
    int inodeBlock = findNextAvailableBlock(disk);
    writeBlock(disk, inodeBlock, inode);
    reserveBlock(inodeBlock, disk);
    updateInodeMap(disk, inodeBlock);

    return inode;
    free(inode);
}

int writeFile(FILE* disk, char* data){
  char* inode = createFile(disk);
  int blockToUse = findNextAvailableBlock(disk);
  addFileSizeToInode(inode, blockToUse);
  writeBlock(disk, blockToUse, data);
  reserveBlock(blockToUse, disk);
  return 0;
}



int main (int argc, char* argv[]){
  createDisk();

  //this is for super block
  FILE* disk = fopen("../disk/vdisk", "r+b");
  writeBlock(disk, 0, "ABBA40962000"); //should use variables but placeholder for now

  //char freeBlock[4096] = {0b00000000, 0b00111111, [2 ... 511] = 0b11111111 };
  char freeBlock[4096] = {[0 ... 511] = 0b11111111 };


  writeBlock(disk, 1, freeBlock);
  int i;
  //this reserves first x blocks x is currently 10 do to spec
  for(i=0; i<10; i++)
    reserveBlock(i, disk);
/*
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


  printf("The next free block is expect 10: %d\n", nextFreeBlock);

  reserveBlock(nextFreeBlock, disk);
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 11: %d\n", nextFreeBlock);
  reserveBlock(nextFreeBlock, disk);
  nextFreeBlock = findNextAvailableBlock(disk);
  reserveBlock(nextFreeBlock, disk);
  nextFreeBlock = findNextAvailableBlock(disk);
  reserveBlock(nextFreeBlock, disk);
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 15: %d\n", nextFreeBlock);


  markBlockFree(12, disk);
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("The next free block is expect 12: %d\n", nextFreeBlock);

  */
  createInodeMap(disk);

  fclose(disk);
  //this is for super block
  //FILE* disk = fopen("../disk/vdisk", "w+b");
  //writeBlock(disk, 1, "0000");
  //fclose(disk);







}
