#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../disk/disk.h"

int MAX_INODES = 1024;
/*
*Code taken from tutorial 9
*/
#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i) //convert to hex digit
//helper useless now
int hexToInteger(char hexValue){
  int addr;
  if(hexValue >= '0' && hexValue<='9')
    addr = hexValue - 0x30;
    else
     {
      printf("TESTING %x\n", hexValue);
         switch(hexValue)
         {
           case (char)0x01: case (char)0x10: addr=1; break;
           case (char)0x02: case (char)0x20: addr=2; break;
           case (char)0x03: case (char)0x30: addr=3; break;
           case (char)0x04: case (char)0x40: addr=4; break;
           case (char)0x05: case (char)0x50: addr=5; break;
           case (char)0x06: case (char)0x60: addr=6; break;
           case (char)0x07: case (char)0x70: addr=7; break;
           case (char)0x08: case (char)0x80: addr=8; break;
           case (char)0x09: case (char)0x90: addr=9; break;
           case (char)0x0A: case (char)0xA0: addr=10; break;
           case (char)0x0B: case (char)0xB0: addr=11; break;
           case (char)0x0C: case (char)0xC0: addr=12; break;
           case (char)0x0D: case (char)0xD0: addr=13; break;
           case (char)0x0E: case (char)0xE0: addr=14; break;
           case (char)0x0F: case (char)0xF0: addr=15; break;
             default: addr=0;
         }
     }
     return addr;
}
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// THIS IS THE INODE AREA
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

//127 in 2
int findInodeFromMap(FILE* disk, int iNode){
  int whichBlock = 2 + iNode /128;
  int addr = 0 ;

  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  readBlock(disk, whichBlock, buffer);
  whichBlock = 4*(iNode%128);

  char tmp[3];
  tmp[0] = buffer[whichBlock+2];
  tmp[1] = buffer[whichBlock+3];
  tmp[2] = '\0';

  free(buffer);
  return strtol(tmp, NULL, 16);
}

int findBlockFromInode(FILE* disk, int inodeAddress){
  char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
  readBlock(disk, inodeAddress, buffer);
  char tmp[3];
  tmp[0] = buffer[8];
  tmp[1] = buffer[9];
  tmp[2] = '\0';

  return strtol(tmp, NULL, 16);
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
      buffer[j*4] = TO_HEX(( (j+((i-2)*128) &0xF0) >>4));
      buffer[j*4+1] = TO_HEX(( (j+((i-2)*128) &0x0F)));
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

        if(buffer[j] == 0 && buffer[j+1] == 0){
        //  printf("Free iNode: %d\n", (j/4+(i-2)*128));
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

  char hex[2];
  hex[0] = (address) >>8;
  hex[1] = (address) & 0xff;

  //maths
  //inode 2 is block 8,9 inode 3 is

  buffer[inode*4+2] = TO_HEX(((address&0xF0)>>4));
  buffer[inode*4+3] = TO_HEX(((address&0x0F)));

  writeBlock(disk, whichBlock, buffer);

  return 0;

}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
// INODE DATA AREA
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************


/*
· Each inode is 32 bytes long
· First 4 bytes: size of file in bytes; an integer
· Next 4 bytes: flags – i.e., type of file (flat or directory); an integer
· Next 2 bytes, multiplied by 10: block numbers for file’s first ten blocks
· Next 2 bytes: single-indirect block
· Last 2 bytes: double-indirect block
*/
char* createEmptyInode(){
  char* inode = malloc(32);
  return inode;
}

//bytes 0-3
void addFileSizeToInode(char* inode, int size){

  inode[0] = size/1000;
  inode[1] = size/100 %10;
  inode[2] = size/10;
  inode[3] = size%10;
}
//bytes 4-7
void addFlagsToInode(char* inode, int fileDir){
  int i;
  for(i=4; i<8; i++)
    inode[i] = fileDir;
  //file=0000, dir=11111
}
void addAddressToInode(char* inode, int address){
  //char hex[2];

  int i;
  for (i=8; i<28;i+=2){
    if (inode[i]==0 && inode[i+1] ==0){
      inode[i]=TO_HEX(((address&0xF0)>>4));
      inode[i+1]=TO_HEX(((address&0x0F)));
      return;
    }
  }
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
// DIRECTORY STUFF
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************


//this just adds to root for now.
//in theory changing 10 to other numbres will work?
int addFileToDirectory(FILE* disk, char* filename, int inodeNum){
  char* buffer = malloc (sizeof(char) * BLOCK_SIZE);
  readBlock(disk, 10, buffer);
  int i,j;
  for(i=0; i<512; i+=32){
    if(buffer[i] == 0 && buffer[i+1] == 0 && buffer[i+2] == 0 && buffer[i+3] ==0){
      printf("any chance i get in here?\n");
      buffer[i]   = inodeNum/1000;
      buffer[i+1] = inodeNum/100 %10;
      buffer[i+2] = inodeNum/10;
      buffer[i+3] = inodeNum%10;
      for(j=0; j<strlen(filename); j++){
        buffer[i+j+4] = filename[j];
      }
      buffer[i+j+4] = '\0';
      writeBlock(disk, 10, buffer);
      return 0;
    }
  }
  return -1;
}
//return the inode of the file?
int findFileInDirectory(FILE* disk, int directoryInode, char* fileName){
  char* buffer = malloc (sizeof(char) * BLOCK_SIZE);
  char* filenameInDirectory = malloc(sizeof(char) * 28);
  int i,j, found;
  found =-1;

  //get which block from findInode

  //for now
  readBlock(disk, 10, buffer);

  for(i=0; i<16; i++){
    for(j=4;j<32;j++){
      if(buffer[32*i+j] != 0){
        //printf("do i happen? Okay wait... %c\n", buffer[32*i+j]);
        filenameInDirectory[j-4] = buffer[32*i+j];
        //printf("just checking: %c\n", filenameInDirectory[j-4]);
      }
      else{
        filenameInDirectory[j-4] = '\0';
      }
      if(strcmp(filenameInDirectory, fileName) == 0 ){
        found =1;
        //printf("almost 100 percent this doesn't happen\n");
        break;
      }
    }
    if(found == 1) break; //ugly double loop exit without a goto
  }
  //printf("This is probably a bad idea: %s\n", filenameInDirectory);

  //convert 4 byte inode# to integer
  if(found ==1){
    int tmp =0;
    tmp += buffer[i*32]*1000;
    tmp += buffer[i*32+1]*100;
    tmp += buffer[i*32+2]*10;
    tmp += buffer[i*32+3]%10;
    return tmp;
  }

  return -1; //didnt find it
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
//dont need this
char* createFile(FILE* disk) {
    char* inode = createEmptyInode();
    // Add more things to inode?
    int inodeBlock = findNextAvailableBlock(disk);
    int whichInode = findNextAvailableInode(disk);

    //add what inode info we can here before writing it

    writeBlock(disk, inodeBlock, inode);
    reserveBlock(inodeBlock, disk);
    updateInodeMap(disk, inodeBlock, whichInode);

    return inode;
}

int writeFile(FILE* disk, char* data, char* filename){
  //need to split filename at / for directory?
  //char* inode = createFile(disk);
  char* inode = createEmptyInode();
  int inodeNum = findNextAvailableInode(disk);
  int inodeBlock = findNextAvailableBlock(disk);
  updateInodeMap(disk, inodeBlock, inodeNum);
  addFileSizeToInode(inode, strlen(data));
  reserveBlock(inodeBlock, disk); //so that the file doesn't take it

  int howManyBlocks = strlen(data)/BLOCK_SIZE;
  int i;

  int blockToUse[howManyBlocks+1];
  for(i=0; i<=howManyBlocks; i++){
    blockToUse[i] = findNextAvailableBlock(disk);
    reserveBlock(blockToUse[i], disk);
    //printf("File will use block: %d\n", blockToUse[i]);
  }
  addFlagsToInode(inode, 0);
  for(i=0; i<=howManyBlocks; i++){
    addAddressToInode(inode, blockToUse[i]);
  }
  writeBlock(disk, inodeBlock, inode); //after adding data to inode
  //char* blocksToWrite = malloc (sizeof(char) * BLOCK_SIZE);
  for(i=0; i<=howManyBlocks; i++){
    //printf("This seems risky\n\t%s\n", data+(i*512));
    writeBlock(disk, blockToUse[i], data+(i*512));
  }
  //printf("I assume we break here?\n");
  addFileToDirectory(disk, filename, inodeNum);

  return 0;
}

//slow transition to separate test files
/*
int main (int argc, char* argv[]){
  printf("Initalize disk, inode map, pre set early blocks\n");
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

  reserveBlock(10,disk); //reserve root block for now

  createInodeMap(disk);
  updateInodeMap(disk, 10, 1);


  int nextFreeBlock = findNextAvailableBlock(disk);

  printf("Create a few files with a little bit of data\n");
  writeFile(disk, "PLEASE WORK", "file-a");
  nextFreeBlock = findNextAvailableBlock(disk);

  writeFile(disk, "WHAT IF YOU ARE DIFFERENT", "file-b");
  nextFreeBlock = findNextAvailableBlock(disk);

  writeFile(disk, "PLEASE WORK", "file-c");
  nextFreeBlock = findNextAvailableBlock(disk);


  printf("After 3 files expect block 16 free?: %d\n", nextFreeBlock);

  printf("just reserve a block for testing\n");
  reserveBlock(nextFreeBlock, disk);
  nextFreeBlock = findNextAvailableBlock(disk);
  printf("17 should be next free now: %d\n", nextFreeBlock);

  printf("we wrote 3 files so expect next inode to be 5?: %d\n", findNextAvailableInode(disk) );

  fclose(disk);
  //this is for super block
  //FILE* disk = fopen("../disk/vdisk", "w+b");
  //writeBlock(disk, 1, "0000");
  //fclose(disk);

}
*/
