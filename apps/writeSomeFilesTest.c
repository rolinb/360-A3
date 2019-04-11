#include "../io/File.h"
#include "../disk/disk.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
  printf("Getting Previously created disk:\n");

  FILE* disk = fopen("disk/vdisk", "r+b");
  int freeBlock = findNextAvailableBlock(disk);
  printf("Only block taken should be root expected free 11: %d\n", freeBlock);
  int freeInode = findNextAvailableInode(disk);
  printf("Expected free iNode is 2: %d\n", freeInode);

  printf("Create a test file with minimal text\n");
  writeFile(disk, "test file 1 under length easily", "file-a");
  freeBlock = findNextAvailableBlock(disk);
  freeInode = findNextAvailableInode(disk);
  printf("File created expected free block 13: %d\n", freeBlock);
  printf("Expected next inode 3: %d\n", freeInode);

  printf("The test i expect to fail... where's the file:\n");
  int inodeFromDirectory = findFileInDirectory(disk, 1, "file-a");
  printf("2 is expected since its the first file: %d\n", inodeFromDirectory);

  printf("Now we create another file to make sure it wasn't all a fluke.\n\n\n");

  printf("Create a test file with minimal text\n");
  writeFile(disk, "test file 2 lorem ipsum whatever", "file-b");
  freeBlock = findNextAvailableBlock(disk);
  freeInode = findNextAvailableInode(disk);
  printf("File created expected free block 15: %d\n", freeBlock);
  printf("Expected next inode 4: %d\n", freeInode);

   inodeFromDirectory = findFileInDirectory(disk, 1, "file-b");
  printf("3 is expected since its the first file: %d\n", inodeFromDirectory);







}
