#include <stdio.h>
#include <stdlib.h>

/*
*Code taken from tutorial 9
*/

const int BLOCK_SIZE = 512;
const int NUM_BLOCKS = 4096;

int main (int argc, char* argv[]){
  FILE* disk = fopen("vdisk", "wb"); //open file to be written for vdisk
  char* init = calloc(BLOCK_SIZE*NUM_BLOCKS, 1);
  fwrite(init, BLOCK_SIZE*NUM_BLOCKS, 1, disk);

  fclose(disk);
  return 0;
}
