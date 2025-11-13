#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 128
#define TOTAL_BLOCKS 1024
#define MAX_NAME_SIZE 50
#define MAX_INPUT_SIZE 5000
#define MAX_DIRECTORIES 100

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

typedef struct FreeBlock
{
  int address;
  struct FreeBlock *next;
  struct FreeBlock *previous;

} FreeBlock;

int freeBlockLength(FreeBlock *head)
{
  int length = 0;
  while (head != NULL)
  {
    length++;
    head = head->next;
  }
  return length;
}

void printFreeBlocks(FreeBlock *head)
{
  while (head != NULL)
  {
    printf("%d -> ", head->address);
    head = head->next;
  }
  printf("\n");
}

int initializeFreeBlocks(FreeBlock **head, FreeBlock **tail)
{
  if (head == NULL || tail == NULL)
  {
    return 0;
  }
  if (*head != NULL || *tail != NULL)
  {
    printf("Free Blocks array not empty!\n");
    return 0;
  }

  FreeBlock *previousNode = NULL;
  for (int index = 0; index < TOTAL_BLOCKS; index++)
  {
    FreeBlock *newNode = malloc(sizeof *newNode);
    if (newNode == NULL)
    {
      printf("Memory allocation failed!\n");
      return 0;
    }
    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->address = index;

    if (*head == NULL)
    {
      *head = newNode;
      *tail = newNode;
      newNode->previous = previousNode;
    }
    else
    {
      newNode->previous = previousNode;
      previousNode->next = newNode;
      *tail = newNode;
    }
    previousNode = newNode;
  }
  return 1;
}

int deleteFreeBlockFromHead(FreeBlock **head, FreeBlock **tail)
{
  if (head == NULL || tail == NULL)
  {
    return 0;
  }

  if (*head == NULL)
  {
    return 0;
  }

  if ((*head)->next == NULL)
  {
    free(*head);
    *head = NULL;
    *tail = NULL;
    return 1;
  }

  FreeBlock *nextNode = (*head)->next;
  nextNode->previous = NULL;
  free(*head);
  *head = nextNode;
  return 1;
}

int insertFreeBlockAtTail(FreeBlock **head, FreeBlock **tail, int address)
{
  if (head == NULL || tail == NULL)
  {
    return 0;
  }

  FreeBlock *newNode = malloc(sizeof *newNode);
  if (newNode == NULL)
  {
    printf("Memory allocation failed!\n");
    return 0;
  }
  newNode->next = NULL;
  newNode->previous = NULL;
  newNode->address = address;

  if (*head == NULL && *tail == NULL)
  {
    *head = newNode;
    *tail = newNode;
    return 1;
  }
  else if (*head == NULL || *tail == NULL)
  {
    free(newNode);
    printf("Invalid head and tail pointers!\n");
    return 0;
  }

  (*tail)->next = newNode;
  newNode->previous = *tail;
  *tail = newNode;
  return 1;
}

void printCircularLinkedList(FileNode *head)
{
  if (head == NULL)
  {
    return;
  }

  FileNode *current = head;
  do
  {
    if (current->isDirectory == 1)
    {
      printf("%s/\n", current->name);
    }
    else
    {
      printf("%s\n", current->name);
    }
    current = current->next;
  } while (current != head);
}

FileNode *insertFileNodeAtTail(FileNode *head, FileNode *parent, char name[], int isDirectory)
{
  FileNode *newNode = malloc(sizeof *newNode);
  if (newNode == NULL)
  {
    printf("Memory allocation failed!\n");
    return head;
  }
  newNode->next = NULL;
  newNode->previous = NULL;
  newNode->parent = parent;
  newNode->child = NULL;
  newNode->isDirectory = isDirectory;
  newNode->totalBlocks = 0;
  strncpy(newNode->name, name, MAX_NAME_SIZE - 1);

  if (head == NULL)
  {
    newNode->next = newNode;
    newNode->previous = newNode;
    return newNode;
  }

  FileNode *tail = head->previous;

  tail->next = newNode;
  newNode->previous = tail;
  newNode->next = head;
  head->previous = newNode;

  return head;
}

FileNode *insertFileNodeAsChild(FileNode *head, char name[], int isDirectory)
{

  if (head == NULL)
  {
    printf("Error: Invalid head, can't add child!\n");
    return NULL;
  }

  if (head->child == NULL)
  {
    FileNode *childHead = insertFileNodeAtTail(NULL, head, name, isDirectory);
    head->child = childHead;
  }
  else
  {
    FileNode *childHead = head->child;
    head->child = insertFileNodeAtTail(childHead, head, name, isDirectory);
  }

  return head;
}

int deleteFileNodeAtHead(FileNode *head, FreeBlock **freeBlockHead, FreeBlock **freeBlockTail, char virtualDisk[][BLOCK_SIZE])
{
  if (head == NULL)
  {
    return 0;
  }

  // Single Node case
  if (head->next == head)
  {

    if (head->child != NULL)
    {
      printf("can't delete Directory, delete sub-folder and files first!\n");
      return 0;
    }

    if (head->parent)
    {
      head->parent->child = NULL;
    }

    // free the blockPointers
    if (head->isDirectory != 1)
    {
      if (freeBlockHead == NULL || freeBlockTail == NULL || virtualDisk == NULL)
      {
        printf("Error: Invalid parameters!\n");
        return 0;
      }
      for (int index = 0; index < head->totalBlocks; index++)
      {
        int address = head->blockPointers[index];
        insertFreeBlockAtTail(freeBlockHead, freeBlockTail, address);

        // Free the virtual disk block
        strcpy(virtualDisk[address], "");
      }
    }

    free(head);
    head = NULL;
    return 1;
  }

  else
  {
    if (head->child != NULL)
    {
      printf("can't delete Directory, delete sub-folder and files first!\n");
      return 0;
    }

    if (head->parent)
    {
      if (head->parent->child == head)
      {
        head->parent->child = head->next;
      }
    }

    if (head->isDirectory != 1)
    {
      if (freeBlockHead == NULL || freeBlockTail == NULL || virtualDisk == NULL)
      {
        printf("Error: Invalid parameters!\n");
        return 0;
      }
      for (int index = 0; index < head->totalBlocks; index++)
      {
        int address = head->blockPointers[index];
        insertFreeBlockAtTail(freeBlockHead, freeBlockTail, address);

        strcpy(virtualDisk[address], "");
      }
    }

    FileNode *previousNode = head->previous;
    FileNode *nextNode = head->next;

    previousNode->next = nextNode;
    nextNode->previous = previousNode;

    free(head);
    head = NULL;
    return 1;
  }
  return 0;
}

FileNode *searchFileInDirectory(FileNode *currentDirectory, char *fileName)
{
  if (currentDirectory == NULL)
  {
    return NULL;
  }
  if (currentDirectory->isDirectory == 0 || currentDirectory->child == NULL)
  {
    return NULL;
  }

  FileNode *childHead = currentDirectory->child;
  FileNode *temp = childHead;

  do
  {
    if (strcmp(temp->name, fileName) == 0)
    {
      return temp;
    }
    temp = temp->next;
  } while (temp != childHead);

  return NULL;
}

void printWorkingDirectory(FileNode *currentDirectory)
{
  if (currentDirectory == NULL)
  {
    return;
  }

  if (currentDirectory->isDirectory == 0)
  {
    currentDirectory = currentDirectory->parent;
  }

  char directoryList[MAX_DIRECTORIES][MAX_NAME_SIZE];
  int index = 0;

  while (currentDirectory != NULL)
  {
    if (index >= MAX_DIRECTORIES)
    {
      printf("\nError: Directory path too deep to print.\n");
      break;
    }
    strcpy(directoryList[index], currentDirectory->name);

    currentDirectory = currentDirectory->parent;
    index++;
  }
  index--;

  while (index >= 0)
  {
    if (strcmp(directoryList[index], "/") == 0)
    {
      printf("%s", directoryList[index]);
    }
    else
    {
      printf("%s/", directoryList[index]);
    }
    index--;
  }
}

void makeDirectory(FileNode **currentDirectory, char name[MAX_NAME_SIZE])
{
  if (*currentDirectory == NULL)
  {
    return;
  }

  FileNode *searchResult = searchFileInDirectory(*currentDirectory, name);
  if (searchResult != NULL)
  {
    printf("directory '%s' already exists\n", name);
    return;
  }

  *currentDirectory = insertFileNodeAsChild(*currentDirectory, name, 1);
  printf("Directory '%s' created successfully\n", name);
}

void removeDirectory(FileNode **currentDirectory, char searchName[MAX_NAME_SIZE])
{
  if (currentDirectory == NULL)
  {
    return;
  }
  if (*currentDirectory == NULL)
  {
    return;
  }

  if (strcmp((*currentDirectory)->name, searchName) == 0)
  {
    if (strcmp(searchName, "/") == 0)
    {
      printf("Can't delete the root directory\n");
      return;
    }

    FileNode *parent = (*currentDirectory)->parent;
    int result = deleteFileNodeAtHead(*currentDirectory, NULL, NULL, NULL);
    if (result == 1)
    {
      *currentDirectory = parent;
      printf("Directory '%s' removed successfully\n", searchName);
    }
    return;
  }

  // Search for directory amongst children
  FileNode *foundDirectory = searchFileInDirectory(*currentDirectory, searchName);
  if (foundDirectory == NULL)
  {
    printf("Directory '%s' not found\n", searchName);
    return;
  }

  if (foundDirectory->isDirectory == 1)
  {
    int result = deleteFileNodeAtHead(foundDirectory, NULL, NULL, NULL);
    if (result == 1)
    {
      printf("Directory '%s' removed successfully\n", searchName);
    }
  }
  else
  {
    printf("Directory '%s' not found\n", searchName);
  }
}

void deleteFile(FileNode **currentDirectory, char searchName[MAX_NAME_SIZE], FreeBlock **freeBlockHead, FreeBlock **freeBlockTail, char virtualDisk[][BLOCK_SIZE])
{
  if (currentDirectory == NULL)
  {
    return;
  }
  if (*currentDirectory == NULL)
  {
    return;
  }

  FileNode *foundFile = searchFileInDirectory(*currentDirectory, searchName);
  if (foundFile == NULL)
  {
    printf("File '%s' not found\n", searchName);
    return;
  }
  if (foundFile->isDirectory == 0)
  {
    int result = deleteFileNodeAtHead(foundFile, freeBlockHead, freeBlockTail, virtualDisk);
    if (result == 1)
    {
      printf("File '%s' deleted successfully\n", searchName);
      return;
    }
  }
  else
  {
    printf("File '%s' not found\n", searchName);
  }
}

void createFile(FileNode **currentDirectory, char name[MAX_NAME_SIZE])
{
  if (*currentDirectory == NULL)
  {
    return;
  }
  FileNode *searchResult = searchFileInDirectory(*currentDirectory, name);
  if (searchResult != NULL)
  {
    printf("file '%s' already exists\n", name);
    return;
  }

  *currentDirectory = insertFileNodeAsChild(*currentDirectory, name, 0);
  printf("file '%s' created successfully\n", name);
}

void changeDirectory(FileNode **currentDirectory, char search[MAX_NAME_SIZE])
{
  if (currentDirectory == NULL)
  {
    return;
  }
  if (*currentDirectory == NULL)
  {
    return;
  }

  if (strcmp(search, "..") == 0)
  {
    if ((*currentDirectory)->parent != NULL)
    {
      *currentDirectory = (*currentDirectory)->parent;
      printf("Moved to %s\n", (*currentDirectory)->name);
      return;
    }
    else
    {
      printf("can't move back from %s\n", (*currentDirectory)->name);
      return;
    }
  }

  FileNode *foundDirectory = searchFileInDirectory(*currentDirectory, search);
  if (foundDirectory == NULL)
  {
    printf("Directory %s not found\n", search);
  }
  if (foundDirectory->isDirectory == 1)
  {
    *currentDirectory = foundDirectory;
    printf("Moved to ");
    printWorkingDirectory(*currentDirectory);
    printf("\n");
    return;
  }

  printf("Directory not found\n");
}

void writeFile(FileNode *file, char *str, FreeBlock **head, FreeBlock **tail, char virtualDisk[][BLOCK_SIZE])
{
  if (file == NULL)
  {
    printf("Invalid file!\n");
    return;
  }

  if (file->isDirectory != 0)
  {
    printf("Can't write! Invalid File\n");
    return;
  }

  int length = strlen(str);
  int blocksAllocated = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;

  if (length > 0 && length < BLOCK_SIZE * TOTAL_BLOCKS)
  {
    if (freeBlockLength(*head) < blocksAllocated)
    {
      printf("not enough memory\n");
      return;
    }
    if (*head == NULL)
    {
      printf("Error: Memory full\n");
      return;
    }

    file->fileSize = length;

    for (int index = 0; index < blocksAllocated; index++)
    {
      int blockIndex = (*head)->address;

      // updating the virtual disk
      strncpy(virtualDisk[blockIndex], str + (index * BLOCK_SIZE), BLOCK_SIZE);

      // Writing freeBlocks to Block Pointers
      int address = file->totalBlocks;
      file->blockPointers[address] = blockIndex;
      file->totalBlocks += 1;

      deleteFreeBlockFromHead(head, tail);
    }
    printf("Data written successfully (size=%d bytes)\n", length);
  }
  else
  {
    printf("Data larger than memory...file not written\n");
    return;
  }
}

void readFile(FileNode *file, char virtualDisk[][BLOCK_SIZE])
{
  for (int index = 0; index < file->totalBlocks; index++)
  {
    int address = file->blockPointers[index];
    char blockData[BLOCK_SIZE] = {0};
    strncpy(blockData, virtualDisk[address], BLOCK_SIZE);

    printf("%s", blockData);
  }
  printf("\n");
}

void getDiskUsage(FreeBlock *head)
{
  printf("Total Blocks: %d\n", TOTAL_BLOCKS);
  int totalFreeBlocks = 0;

  while (head != NULL)
  {
    totalFreeBlocks++;
    head = head->next;
  }

  float diskUsage = 1 - (float)totalFreeBlocks / TOTAL_BLOCKS;
  diskUsage *= 100;

  printf("Used Blocks: %d\n", TOTAL_BLOCKS - totalFreeBlocks);
  printf("Free Blocks: %d\n", totalFreeBlocks);
  printf("Disk Usage: %.2f %%\n", diskUsage);
}

void ListDirectory(FileNode *currentDirectory)
{
  if (currentDirectory == NULL)
  {
    return;
  }
  if (currentDirectory->child == NULL)
  {
    printf("Directory empty");
  }
  else
  {
    printCircularLinkedList(currentDirectory->child);
  }
  printf("\n");
}

void freeFileNode(FileNode *root)
{
  if (root == NULL)
  {
    return;
  }
  FileNode *head = root;
  FileNode *tail = root->previous;
  if (tail == NULL)
  {
    free(head);
    return;
  }
  tail->next = NULL;
  head->previous = NULL;

  while (head != NULL)
  {
    FileNode *nextNode = head->next;
    if (head->child)
    {
      freeFileNode(head->child);
    }
    free(head);
    head = nextNode;
  }
}

void exitProgram(FileNode *root, char virtualDisk[][BLOCK_SIZE], FreeBlock *freeBlockHead, FreeBlock *freeBlockTail)
{
  while (freeBlockHead != NULL)
  {
    FreeBlock *nextNode = freeBlockHead->next;
    free(freeBlockHead);
    freeBlockHead = nextNode;
  }

  freeFileNode(root);

  printf("Memory released. Exiting program...\n");

  exit(1);
}

void handleInput(FileNode *root, FileNode **currentDirectory, char virtualDisk[][BLOCK_SIZE], FreeBlock **freeBlockHead, FreeBlock **freeBlockTail)
{
  char userInput[MAX_INPUT_SIZE];
  printf("%s> ", (*currentDirectory)->name);
  scanf("%[^\n]", userInput);
  getchar();

  char *command = strtok(userInput, " ");
  char *fileName = strtok(NULL, " ");
  char *data = strtok(NULL, "\"");

  if (command == NULL || strlen(command) == 0)
  {
    printf("Invalid command\n");
    return;
  }

  if (strcmp(command, "mkdir") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid directory name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid directory name!\n");
    }
    else
    {
      makeDirectory(currentDirectory, fileName);
    }
  }

  else if (strcmp(command, "cd") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid directory name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid directory name!\n");
    }
    else
    {
      changeDirectory(currentDirectory, fileName);
    }
  }

  else if (strcmp(command, "create") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid file name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      createFile(currentDirectory, fileName);
    }
  }

  else if (strcmp(command, "write") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid file name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid file name!\n");
    }
    if (data == NULL)
    {
      printf("can't write, invalid data\n");
    }
    else
    {
      FileNode *fileToWrite = searchFileInDirectory(*currentDirectory, fileName);
      if (fileToWrite != NULL)
      {

        writeFile(fileToWrite, data, freeBlockHead, freeBlockTail, virtualDisk);
      }
      else
      {
        printf("File %s not found\n", fileName);
      }
    }
  }

  else if (strcmp(command, "read") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid file name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      FileNode *fileToRead = searchFileInDirectory(*currentDirectory, fileName);
      if (fileToRead != NULL)
      {
        readFile(fileToRead, virtualDisk);
      }
      else
      {
        printf("%s not found\n", fileName);
      }
    }
  }

  else if (strcmp(command, "delete") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid file name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      deleteFile(currentDirectory, fileName, freeBlockHead, freeBlockTail, virtualDisk);
    }
  }

  else if (strcmp(command, "rmdir") == 0)
  {
    if (fileName == NULL || strlen(fileName) == 0)
    {
      printf("Invalid directory name!\n");
      return;
    }
    if (strcmp(fileName, "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      removeDirectory(currentDirectory, fileName);
    }
  }

  else if (strcmp(command, "pwd") == 0)
  {
    printWorkingDirectory(*currentDirectory);
    printf("\n");
  }

  else if (strcmp(command, "ls") == 0)
  {
    ListDirectory(*currentDirectory);
  }

  else if (strcmp(command, "df") == 0)
  {
    getDiskUsage(*freeBlockHead);
  }

  else if (strcmp(command, "exit") == 0)
  {
    exitProgram(root, virtualDisk, *freeBlockHead, *freeBlockTail);
  }
  else
  {
    printf("%s is not recognized as a valid command\n", command);
  }
}

int main()
{

  char virtualDisk[TOTAL_BLOCKS][BLOCK_SIZE];
  if (sizeof(virtualDisk) == 0)
  {
    printf("Error initializing Virtual Disk\n");
    return 1;
  }

  FreeBlock *freeBlockHead = NULL;
  FreeBlock *freeBlockTail = NULL;
  if (initializeFreeBlocks(&freeBlockHead, &freeBlockTail) == 0)
  {
    printf("Error initializing Memory\n");
    return 1;
  }

  FileNode *root = NULL;
  root = insertFileNodeAtTail(root, NULL, "/", 1);
  FileNode *currentDirectory = root;
  if (root == NULL)
  {
    printf("Error starting VFS\n");
    return 1;
  }

  printf("$ ./vfs \n");
  printf("Compact VFS - ready. Type 'exit' to quit. \n");

  while (1)
  {
    handleInput(root, &currentDirectory, virtualDisk, &freeBlockHead, &freeBlockTail);
  }
  return 0;
}