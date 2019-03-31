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

char* addBlock(char* inode, int block) {
    // You're going to probably want a bunch of functions like these
}

int main(int argc, char* argv[]) {
    FILE* disk = fopen("vdisk", "w+b");
    char* myInode = createEmptyInode();
    // Maybe add more things to the inode
    writeBlock(disk, 2, myInode);

    free(myInode);
    fclose(disk);
    return 0;
}