#include <stdio.h>
#include <stdlib.h>
#include "../disk/disk.h"

int MAX_INODES = 1024;
/*
*Code taken from tutorial 9
*/
#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i) //convert to hex digit

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// THIS IS THE INODE AREA
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

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
  int i,j;
  char hex[2];
  //unsigned char  tmp, tmp2;
  //set up map? in a janky fashion
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);

  for (i=2; i<10; i++){
    readBlock(disk, i, buffer);
    for(j=0; j<128;j++){
      hex[0] = (j + ((i-2)*128)) >>8;
      hex[1] = (j+ ((i-2)*128)) & 0xff;
      buffer[j*4] = hex[0];
      buffer[j*4+1] = hex[1];
    }
    writeBlock(disk, i, buffer);
  }
}



int findNextAvailableInode(FILE* disk){
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  int i,j;
  for (i=2; i<10; i++){
    readBlock(disk, i, buffer);
    for(j=1; j<512;j++){
      if(i==2 && j<7) continue; //bypassing the first couple reserved inodes
      if(!(j%2) && j%4){ //i think this gets the ones i want?
        printf("HMM %d\n", buffer[j]);
        printf("mmm %d\n", buffer[j+1]);
        if(buffer[j] == 0 && buffer[j+1] == 0){
          printf("Free iNode: %d\n", (j/4+(i-2)*128));
          return (j/4+(i-2)*128); //this looks awful
        }
      }
        //printf("Should not be empty - %c\n", buffer[j]);

    }
  }
  return 0;
}


//mark the inode map find where and add the address
int updateInodeMap(FILE* disk, int address, int inode){
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  //int i,j;
  int whichBlock = inode/128 + 2;
  if(address > NUM_BLOCKS) return -1; //bail out we should never get here
  readBlock(disk, whichBlock, buffer);

  /*int[4] values;
  value[0] = address/1000;
  value[1] = address/100 %10;
  value[2] = address/10 %10;
  value[3] = address%10;
*/
  char hex[2];
  hex[0] = (address) >>8;
  hex[1] = (address) & 0xff;

  //maths
  //inode 2 is block 8,9 inode 3 is
  buffer[inode*4+2] = hex[0];
  buffer[inode*4+2] = hex[1];

  writeBlock(disk, whichBlock, buffer);




  return 0;

}

void addFileSizeToInode(char* inode, int size){
  inode[0] = size;
}

void addAddressToInode(char* inode, int address){
  inode[8] = address; //first address this isn't good but its a start
}



//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// BLOCK AREA
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************


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

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// FILE STUFF
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

//okay what happens when we create a file?
//we need to get an iNode
// claim a block to write
//todo: possibly more than 1 block
//update inodemap with new inode?

char* createFile(FILE* disk) {
    char* inode = createEmptyInode();
    // Add more things to inode?
    int inodeBlock = findNextAvailableBlock(disk);
    int whichInode = findNextAvailableInode(disk);
    writeBlock(disk, inodeBlock, inode);
    reserveBlock(inodeBlock, disk);
    updateInodeMap(disk, inodeBlock, whichInode);

    return inode;
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


  createInodeMap(disk);
  findNextAvailableInode(disk);
  updateInodeMap(disk, findNextAvailableBlock(disk), findNextAvailableInode(disk));
  findNextAvailableInode(disk);

  fclose(disk);
  //this is for super block
  //FILE* disk = fopen("../disk/vdisk", "w+b");
  //writeBlock(disk, 1, "0000");
  //fclose(disk);







}
