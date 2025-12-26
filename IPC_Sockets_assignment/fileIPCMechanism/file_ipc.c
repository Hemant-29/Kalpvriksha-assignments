#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../utils.h"

void writeArrayToFile(const char *fileName, int *integerArray, int arrayLength)
{
  FILE *filePointer = fopen(fileName, "w");
  if (filePointer == NULL)
  {
    perror("File open failed for writing");
    exit(1);
  }
  fwrite(integerArray, sizeof(int), arrayLength, filePointer);
  fclose(filePointer);
}

void readArrayFromFile(const char *fileName, int *integerArray, int arrayLength)
{
  FILE *filePointer = fopen(fileName, "r");
  if (filePointer == NULL)
  {
    perror("File open failed for reading");
    exit(1);
  }
  fread(integerArray, sizeof(int), arrayLength, filePointer);
  fclose(filePointer);
}

int main()
{
  int integerArray[] = {5, 2, 9, 1, 3};
  pid_t processId = fork();

  if (processId < 0)
  {
    perror("fork failed");
    exit(1);
  }

  if (processId == 0)
  {
    // CHILD PROCESS
    sleep(1); // Wait for parent to write initial data

    readArrayFromFile("data.txt", integerArray, ARRAY_LENGTH);

    sortIntegerArray(integerArray, ARRAY_LENGTH);

    writeArrayToFile("data.txt", integerArray, ARRAY_LENGTH);
  }
  else
  {
    // PARENT PROCESS

    writeArrayToFile("data.txt", integerArray, ARRAY_LENGTH);

    printf("Before Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);

    wait(NULL); // Wait for child to finish sorting

    readArrayFromFile("data.txt", integerArray, ARRAY_LENGTH);

    printf("After Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);
  }
  return 0;
}
