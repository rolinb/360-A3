#include <stdio.h>
#include <stdlib.h>
const int BLOCK_SIZE = 512;
const int NUM_BLOCKS = 4096;

void readBlock(FILE* disk, int blockNum, char* buffer){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, disk);
}

void writeBlock(FILE* disk, int blockNum, char* data){
    fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, disk); // Note: this will overwrite existing data in the block
}

int main(int argc, char* argv[]) {
    FILE* disk = fopen("vdisk", "w+b"); // Notice that we are now reading and writing
    writeBlock(disk, 2, "Hello world!");
    char* buffer = malloc(sizeof(char) * BLOCK_SIZE);
    readBlock(disk, 2, buffer);
    //printf("%s", buffer);
    for (int i = 0; i < BLOCK_SIZE; i++){
        printf("%2x ", buffer[i]);
    }

    free(buffer);
    fclose(disk);
    return 0;
}