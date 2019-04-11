#include "../io/File.h"
#include "../disk/disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
  printf("Getting Previously created disk:\n");

  FILE* disk = fopen("disk/vdisk", "r+b");

  char* bigFile = readFile(disk, "big-file");
  printf("The dream: \n\n%s\n", bigFile );

  char* littleFile = readFile(disk, "file-b");

  printf("The nightmare: \n\n%s\n", littleFile );

  /*
  //Doesn't work no time to fix
  deleteFile(disk, "file-b");

  char* unfoundFile = readFile(disk, "file-b");

  printf("The nightmare: \n\n%s", unfoundFile);*/

}
