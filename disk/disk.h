#include <stdio.h>
#include <stdlib.h>
extern int BLOCK_SIZE;
extern int NUM_BLOCKS;

int createDisk();
void writeBlock(FILE* disk, int blockNum, char* data);
void readBlock(FILE* disk, int blockNum, char* buffer);
