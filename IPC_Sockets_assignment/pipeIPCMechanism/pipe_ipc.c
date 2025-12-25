#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../utils.h"

void createPipe(int *pipeDescriptors)
{
  if (pipe(pipeDescriptors) == -1)
  {
    perror("Pipe creation failed");
    exit(1);
  }
}

void writeArrayToPipe(int fileDescriptor, int *integerArray, int arrayLength)
{
  ssize_t bytesWritten = write(fileDescriptor, integerArray, arrayLength * sizeof(int));
  if (bytesWritten == -1)
  {
    perror("Pipe write failed");
    exit(1);
  }
}

void readArrayFromPipe(int fileDescriptor, int *integerArray, int arrayLength)
{
  ssize_t bytesRead = read(fileDescriptor, integerArray, arrayLength * sizeof(int));
  if (bytesRead <= 0)
  {
    perror("Pipe read failed or EOF");
    exit(1);
  }
}

int main()
{
  int integerArray[ARRAY_LENGTH] = {7, 3, 8, 2, 5};

  int parentToChildPipe[2];
  int childToParentPipe[2];

  createPipe(parentToChildPipe);
  createPipe(childToParentPipe);

  pid_t processId = fork();
  if (processId < 0)
  {
    perror("fork failed");
    exit(1);
  }

  if (processId == 0)
  {
    // Close unused ends
    close(parentToChildPipe[1]);
    close(childToParentPipe[0]);

    readArrayFromPipe(parentToChildPipe[0], integerArray, ARRAY_LENGTH);

    sortIntegerArray(integerArray, ARRAY_LENGTH);

    writeArrayToPipe(childToParentPipe[1], integerArray, ARRAY_LENGTH);

    // Close the remaining ends
    close(parentToChildPipe[0]);
    close(childToParentPipe[1]);
  }
  else
  {
    close(parentToChildPipe[0]);
    close(childToParentPipe[1]);

    printf("Before Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);

    writeArrayToPipe(parentToChildPipe[1], integerArray, ARRAY_LENGTH);

    wait(NULL);

    readArrayFromPipe(childToParentPipe[0], integerArray, ARRAY_LENGTH);

    printf("After Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);

    close(parentToChildPipe[1]);
    close(childToParentPipe[0]);
  }

  return 0;
}
