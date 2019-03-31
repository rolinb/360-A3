#include <stdio.h>
#include <stdlib.h>
#include "disk.h"

int BLOCK_SIZE = 512;
int NUM_BLOCKS = 4096;
/*
*Code taken from tutorial 9
*/



int createDisk(){
  FILE* disk = fopen("../Disk/vdisk", "wb"); //open file to be written for vdisk
  char* init = calloc(BLOCK_SIZE*NUM_BLOCKS, 1);
  fwrite(init, BLOCK_SIZE*NUM_BLOCKS, 1, disk);

  fclose(disk);
  return 0;
}

void writeBlock(FILE* disk, int blockNum, char* data){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET); // Find the block to write to
    fwrite(data, BLOCK_SIZE, 1, disk); // Note: this will overwrite existing data in the block
}

void readBlock(FILE* disk, int blockNum, char* buffer){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, disk);
}
/*
int main (int argc, char* argv[]){

  return 0;
}*/
