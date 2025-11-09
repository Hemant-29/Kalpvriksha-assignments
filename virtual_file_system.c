#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define TOTAL_BLOCKS 1024
#define MAX_NAME_SIZE 50
#define MAX_INPUT 100
#define MAX_INPUT_TOKENS 5

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

// __FreeBlocks functions__

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
    // Create new Node
    FreeBlock *newNode = malloc(sizeof *newNode);
    if (newNode == NULL)
    {
      printf("Memory allocation failed!\n");
      return 0;
    }
    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->address = index;

    // Assign newNode
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

  // Single Node case
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

  // Create New Node
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

// __FileNode functions__

int getLengthCircularLinkedList(FileNode *head)
{
  if (head == NULL)
  {
    return 0;
  }

  int length = 0;
  FileNode *current = head;

  do
  {
    length++;
    current = current->next;
  } while (current != head);

  return length;
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

void printAllFiles(FileNode *head)
{
  if (head == NULL)
  {
    return;
  }

  FileNode *current = head;

  while (current->next != head)
  {
    printf("%s -> ", current->name);
    printAllFiles(current->child);
    current = current->next;
  }
  printf("%s\n", current->name);
}

FileNode *insertFileNodeAtTail(FileNode *head, FileNode *parent, char name[], int isDirectory)
{
  // Create a new Node
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
  strcpy(newNode->name, name);

  if (head == NULL)
  {
    newNode->next = newNode;
    newNode->previous = newNode;
    return newNode;
  }

  FileNode *tail = head->previous;

  // Insert this newNode between tail and head
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

  // __Single Node case__
  if (head->next == head)
  {
    // Check for child Nodes
    if (head->child != NULL)
    {
      printf("can't delete Directory, delete sub-folder and files first!\n");
      return 0;
    }
    // Update Parent Node
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

    // Delete
    free(head);
    return 1;
  }

  // __Other cases__
  else
  {
    // Check for child Nodes
    if (head->child != NULL)
    {
      printf("can't delete Directory, delete sub-folder and files first!\n");
      return 0;
    }

    // Update Parent
    if (head->parent)
    {
      if (head->parent->child == head)
      {
        head->parent->child = head->next;
      }
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

    // Deletion
    FileNode *previousNode = head->previous;
    FileNode *nextNode = head->next;

    previousNode->next = nextNode;
    nextNode->previous = previousNode;

    free(head);
    return 1;
  }
  return 0;
}

// __Command Functions__

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

  char directoryList[1000][MAX_NAME_SIZE];
  int index = 0;

  while (currentDirectory != NULL)
  {
    // printf("%s/", currentDirectory->name);
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

  // if name is of current directory
  if (strcmp((*currentDirectory)->name, searchName) == 0)
  {
    // if searchName is root directory
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

  // Search for file amongst children
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
  if (length > 0 && length < BLOCK_SIZE * TOTAL_BLOCKS)
  {
    // Set file size
    file->fileSize = length;
    int blocksAllocated = (length / BLOCK_SIZE) + 1;

    if (*head == NULL)
    {
      printf("Error: Memory full\n");
      return;
    }

    // Write the string to the index of these freeBlocks
    int blockIndex = (*head)->address;
    strcpy(virtualDisk[blockIndex], str);
    for (int index = 0; index < blocksAllocated; index++)
    {
      // Write freeBlocks to Block Pointers

      blockIndex = (*head)->address;
      int address = file->totalBlocks;
      file->blockPointers[address] = blockIndex;
      file->totalBlocks += 1;

      // Remove free Blocks from list
      deleteFreeBlockFromHead(head, tail);
    }
    printf("Data written successfully (size=%d bytes)\n", length);
  }
  else
  {
    printf("Enter valid Data\n");
    return;
  }
}

void readFile(FileNode *file, char virtualDisk[][BLOCK_SIZE])
{
  for (int index = 0; index < file->totalBlocks; index++)
  {
    // __implement__ Read this
    int address = file->blockPointers[index];
    printf("%s ", virtualDisk[address]);
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
  // Freeing all the freeBlocks
  while (freeBlockHead != NULL)
  {
    FreeBlock *nextNode = freeBlockHead->next;
    free(freeBlockHead);
    freeBlockHead = nextNode;
  }

  // Free Virtual Disk
  free(virtualDisk);

  // Free all the fileNodes
  freeFileNode(root);

  exit(1);
}

void handleInput(FileNode *root, FileNode **currentDirectory, char virtualDisk[][BLOCK_SIZE], FreeBlock **freeBlockHead, FreeBlock **freeBlockTail)
{
  // __Taking input__
  char userInput[MAX_INPUT];
  printf("%s> ", (*currentDirectory)->name);
  scanf("%[^\n]", userInput);
  getchar();

  // __Creating a token's array__
  char tokenArray[MAX_INPUT_TOKENS][MAX_INPUT] = {'\0'};
  int tokenIndex = 0;

  char *token = strtok(userInput, " ");
  while (token != NULL && tokenIndex < MAX_INPUT_TOKENS)
  {
    if (token != NULL)
    {
      strcpy(tokenArray[tokenIndex], token);
    }
    token = strtok(NULL, " ");
    tokenIndex++;
  }

  // Printing tokens array
  printf("\tToken array is: ");

  for (int i = 0; i < tokenIndex; i++)
  {
    printf("%s, ", tokenArray[i]);
  }
  printf("\n");

  // __Processing input commands__
  if (strcmp(tokenArray[0], "mkdir") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid directory name!\n");
    }
    else
    {
      makeDirectory(currentDirectory, tokenArray[1]);
    }
  }

  else if (strcmp(tokenArray[0], "cd") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid directory name!\n");
    }
    else
    {
      changeDirectory(currentDirectory, tokenArray[1]);
    }
  }

  else if (strcmp(tokenArray[0], "create") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      createFile(currentDirectory, tokenArray[1]);
    }
  }

  else if (strcmp(tokenArray[0], "write") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      FileNode *fileToWrite = searchFileInDirectory(*currentDirectory, tokenArray[1]);
      if (fileToWrite != NULL)
      {
        writeFile(fileToWrite, tokenArray[2], freeBlockHead, freeBlockTail, virtualDisk);
      }
      else
      {
        printf("File %s not found\n", tokenArray[1]);
      }
    }
  }

  else if (strcmp(tokenArray[0], "read") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      FileNode *fileToRead = searchFileInDirectory(*currentDirectory, tokenArray[1]);
      if (fileToRead != NULL)
      {
        readFile(fileToRead, virtualDisk);
      }
      else
      {
        printf("%s not found\n", tokenArray[1]);
      }
    }
  }

  else if (strcmp(tokenArray[0], "delete") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      deleteFile(currentDirectory, tokenArray[1], freeBlockHead, freeBlockTail, virtualDisk);
    }
  }

  else if (strcmp(tokenArray[0], "rmdir") == 0)
  {
    if (strcmp(tokenArray[1], "") == 0)
    {
      printf("Invalid file name!\n");
    }
    else
    {
      removeDirectory(currentDirectory, tokenArray[1]);
    }
  }

  else if (strcmp(tokenArray[0], "pwd") == 0)
  {
    printWorkingDirectory(*currentDirectory);
    printf("\n");
  }

  else if (strcmp(tokenArray[0], "ls") == 0)
  {
    // _implement_ not worknig properly
    ListDirectory(*currentDirectory);
  }

  else if (strcmp(tokenArray[0], "df") == 0)
  {
    getDiskUsage(*freeBlockHead);
  }

  else if (strcmp(tokenArray[0], "exit") == 0)
  {
    exitProgram(root, virtualDisk, *freeBlockHead, *freeBlockTail);
  }
  else
  {
    printf("%s is not recognized as a valid command\n", tokenArray[0]);
  }
}

int main()
{
  printf("$ ./vfs \n");
  printf("Compact VFS - ready. Type 'exit' to quit. \n");
  char virtualDisk[TOTAL_BLOCKS][BLOCK_SIZE];

  // Initializing free blocks

  FreeBlock *freeBlockHead = NULL;
  FreeBlock *freeBlockTail = NULL;

  initializeFreeBlocks(&freeBlockHead, &freeBlockTail);

  // deleteFreeBlockFromHead(&freeBlockHead, &freeBlockTail);
  // deleteFreeBlockFromHead(&freeBlockHead, &freeBlockTail);
  // deleteFreeBlockFromHead(&freeBlockHead, &freeBlockTail);
  // insertFreeBlockAtTail(&freeBlockHead, &freeBlockTail, 1024);
  // insertFreeBlockAtTail(&freeBlockHead, &freeBlockTail, 1025);
  // printFreeBlocks(freeBlockHead);

  // printf("\nfree blocks created: \n");
  // printf("Head: %d\n", freeBlockHead->address);
  // printf("Tail: %d\n", freeBlockTail->address);

  // Initailize root
  FileNode *root = NULL;
  root = insertFileNodeAtTail(root, NULL, "/", 1);
  FileNode *currentDirectory = root;

  // Add to tail
  root = insertFileNodeAtTail(root, NULL, "file 1", 1);
  root = insertFileNodeAtTail(root, NULL, "file 2", 1);

  // printf("\nLength: %d\n", getLengthCircularLinkedList(head));

  // Add as child
  root = insertFileNodeAsChild(root, "documents", 1);
  root = insertFileNodeAsChild(root, "images", 1);

  // Add as child
  root->child = insertFileNodeAsChild(root->child, "document1.txt", 0);
  root->child->next = insertFileNodeAsChild(root->child->next, "picture.jpg", 0);

  while (1)
  {
    handleInput(root, &currentDirectory, virtualDisk, &freeBlockHead, &freeBlockTail);
  }

  // print Linked list
  // printAllFiles(currentDirectory);

  // print Working Directory
  // printf("\ncwd: \n");
  // printWorkingDirectory(root);

  // changeDirectory(&currentDirectory, "documents");

  // Get disk usage
  // printf("\n");
  // getDiskUsage(freeBlockHead);
  return 0;
}