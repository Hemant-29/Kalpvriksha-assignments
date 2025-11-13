#ifndef VFS
#define VFS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 128
#define TOTAL_BLOCKS 1024
#define MAX_NAME_SIZE 50
#define MAX_INPUT_SIZE 5000
#define MAX_DIRECTORIES 100

// Define freeBlock structure
typedef struct FreeBlock
{
  int address;
  struct FreeBlock *next;
  struct FreeBlock *previous;

} FreeBlock;

// Define FileNode structure
typedef struct FileNode
{
  char name[MAX_NAME_SIZE];
  struct FileNode *next;
  struct FileNode *previous;
  struct FileNode *child;
  struct FileNode *parent;
  int isDirectory;
  int fileSize;
  int blockPointers[TOTAL_BLOCKS];
  int totalBlocks;
} FileNode;

// Free_block.c functions definition
int freeBlockLength(FreeBlock *head);
void printFreeBlocks(FreeBlock *head);
int initializeFreeBlocks(FreeBlock **head, FreeBlock **tail);
int deleteFreeBlockFromHead(FreeBlock **head, FreeBlock **tail);
int insertFreeBlockAtTail(FreeBlock **head, FreeBlock **tail, int address);

// File_node.c functions definition
void printCircularLinkedList(FileNode *head);
FileNode *insertFileNodeAtTail(FileNode *head, FileNode *parent, char name[], int isDirectory);
FileNode *insertFileNodeAsChild(FileNode *head, char name[], int isDirectory);
int deleteFileNodeAtHead(FileNode *head, FreeBlock **freeBlockHead, FreeBlock **freeBlockTail, char virtualDisk[][BLOCK_SIZE]);
FileNode *searchFileInDirectory(FileNode *currentDirectory, char *fileName);

// file_operations.c functions definition
void printWorkingDirectory(FileNode *currentDirectory);
void makeDirectory(FileNode **currentDirectory, char name[MAX_NAME_SIZE]);
void removeDirectory(FileNode **currentDirectory, char searchName[MAX_NAME_SIZE]);
void deleteFile(FileNode **currentDirectory, char searchName[MAX_NAME_SIZE], FreeBlock **freeBlockHead, FreeBlock **freeBlockTail, char virtualDisk[][BLOCK_SIZE]);
void createFile(FileNode **currentDirectory, char name[MAX_NAME_SIZE]);
void changeDirectory(FileNode **currentDirectory, char search[MAX_NAME_SIZE]);
void writeFile(FileNode *file, char *str, FreeBlock **head, FreeBlock **tail, char virtualDisk[][BLOCK_SIZE]);
void readFile(FileNode *file, char virtualDisk[][BLOCK_SIZE]);
void getDiskUsage(FreeBlock *head);
void ListDirectory(FileNode *currentDirectory);
void freeFileNode(FileNode *root);
void exitProgram(FileNode *root, char virtualDisk[][BLOCK_SIZE], FreeBlock *freeBlockHead, FreeBlock *freeBlockTail);

#endif