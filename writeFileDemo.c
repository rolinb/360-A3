#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
const int BLOCK_SIZE = 512;
const int NUM_BLOCKS = 4096;
const int INODE_SIZE = 32;

void readBlock(FILE* disk, int blockNum, char* buffer){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, disk);
}

void writeBlock(FILE* disk, int blockNum, char* data){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, disk); // Note: this will overwrite existing data in the block
}

char* createEmptyInode() {
    char* inode = malloc(32);
    inode[10] = 3;  //First direct block value pointing to block number 3
    return inode;
}

void createFile(FILE* disk) {
    char* inode = createEmptyInode();
    // Add more things to inode?
    writeBlock(disk, 2, inode);
    
    free(inode);
}

void writeToFile(FILE* disk, char* data) {
    char* inodeBuffer = malloc(sizeof(char) * BLOCK_SIZE);
    readBlock(disk, 2, inodeBuffer);
    int fileBlockNumber = inodeBuffer[10];
    writeBlock(disk, fileBlockNumber, data);

    free(inodeBuffer);
}

int main(int argc, char* argv[]) {
    FILE* disk = fopen("vdisk", "w+b");
    createFile(disk);    
    writeToFile(disk, "Hello World!");

    fclose(disk);
    return 0;
}