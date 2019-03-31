#include <stdio.h>
#include <stdlib.h>
const int BLOCK_SIZE = 512;
const int NUM_BLOCKS = 4096;

void writeBlock(FILE* disk, int blockNum, char* data){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET); // Find the block to write to
    fwrite(data, BLOCK_SIZE, 1, disk); // Note: this will overwrite existing data in the block
}

int main(int argc, char* argv[]) {
    FILE* disk = fopen("vdisk", "wb"); // Open the file to be written to in binary mode
    writeBlock(disk, 2, "Hello world!");

    fclose(disk);
    return 0;
}