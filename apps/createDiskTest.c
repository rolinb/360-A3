#include "../io/File.h"
#include "../disk/disk.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
  printf("Initalize disk, inode map, pre set early blocks\n");
  createDisk();

  printf("Disk created:\nCreating Super Block:\n");

  //this is for super block
  FILE* disk = fopen("disk/vdisk", "r+b");
  writeBlock(disk, 0, "ABBA40962000"); //should use variables but placeholder for now

  //char freeBlock[4096] = {0b00000000, 0b00111111, [2 ... 511] = 0b11111111 };
  char freeBlock[4096] = {[0 ... 511] = 0b11111111 };

  printf("Marking off reserved blocks:\n");
  writeBlock(disk, 1, freeBlock);
  int i;
  //this reserves first x blocks x is currently 10 do to spec
  for(i=0; i<10; i++)
    reserveBlock(i, disk);

  reserveBlock(10,disk); //reserve root block for now
  printf("Creating inodeMap:\n");
  createInodeMap(disk);
  printf("Updating inode map for root directory:\n");
  updateInodeMap(disk, 10, 1);



}
