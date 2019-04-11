#include "../io/File.h"
#include "../disk/disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  printf("3 is expected since its the second file: %d\n", inodeFromDirectory);

  int fileBlock = findInodeFromMap(disk, inodeFromDirectory);
  printf("grabbing where the inode is expect 13: %d\n", fileBlock);

  int dataBlock = findBlockFromInode(disk, fileBlock, 0);
  printf("Expected Value: 14 %d\n", dataBlock);


  char* bigBoyFile = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu,";
  printf("Just to check should be 600 %lu\n", strlen(bigBoyFile));
  writeFile(disk, bigBoyFile, "big-file");

 inodeFromDirectory = findFileInDirectory(disk, 1, "big-file");
 printf("4 is expected since its the third file: %d\n", inodeFromDirectory);

 fileBlock = findInodeFromMap(disk, inodeFromDirectory);
 printf("grabbing where the inode is expect 15: %d\n", fileBlock);

 dataBlock = findBlockFromInode(disk, fileBlock, 0);
 printf("Expected Value: 16 %d\n", dataBlock);

 dataBlock = findBlockFromInode(disk, fileBlock, 1);
 printf("Expected Value: 17 %d\n", dataBlock);










}
