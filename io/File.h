#include <stdio.h>
#include <stdlib.h>
int findInodeFromMap(FILE* disk, int iNode);
int hexToInteger(char x);
void createInodeMap(FILE* disk);
int findNextAvailableInode(FILE* disk);
int findBlockFromInode(FILE* disk, int inodeAddress, int blockNum);
int updateInodeMap(FILE* disk, int address, int inode);
char* createEmptyInode();
void addFileSizeToInode(char* inode, int size);
void addFlagsToInode(char* inode, int fileDir);
void addAddressToInode(char* inode, int address);
int findNextAvailableBlock(FILE* disk);
int findNextAvailableBlockAndUseIt(FILE* disk);
int reserveBlock(int blockNum, FILE* disk);
int markBlockFree(int blockNum, FILE* disk);
int addFileToDirectory(FILE* disk, char* filename, int inodeNum);
int findFileInDirectory(FILE* disk, int directoryInode, char* fileName);
char* createFile(FILE* disk) ;
int writeFile(FILE* disk, char* data, char* filename);
char* readFile(FILE* disk, char* fileName);
void deleteFile(FILE* disk, char* fileName);
int deleteFileFromDirectory(FILE* disk, char* fileName);
