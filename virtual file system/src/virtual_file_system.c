#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs_library.h"

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