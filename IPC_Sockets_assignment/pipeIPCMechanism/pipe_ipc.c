  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/wait.h>

  #define ARRAY_LENGTH 5

  void sortIntegerArray(int integerArray[], int arraySize)
  {
    for (int firstIndex = 0; firstIndex < arraySize - 1; firstIndex++)
    {
      for (int secondIndex = firstIndex + 1; secondIndex < arraySize; secondIndex++)
      {
        if (integerArray[firstIndex] > integerArray[secondIndex])
        {
          int temporaryValue = integerArray[firstIndex];
          integerArray[firstIndex] = integerArray[secondIndex];
          integerArray[secondIndex] = temporaryValue;
        }
      }
    }
  }

  void printArray(int *array, int count)
  {
    for (int displayIndex = 0; displayIndex < count; displayIndex++)
    {
      printf("%d ", array[displayIndex]);
    }
    printf("\n");
  }

  int main()
  {
    int integerArray[ARRAY_LENGTH] = {7, 3, 8, 2, 5};

    int parentToChildPipe[2];
    int childToParentPipe[2];

    if (pipe(parentToChildPipe) == -1 || pipe(childToParentPipe) == -1)
    {
      perror("Pipe creation failed");
      exit(1);
    }

    pid_t processId = fork();
    if (processId < 0)
    {
      perror("fork failed");
      exit(1);
    }

    if (processId == 0)
    {
      close(parentToChildPipe[1]);
      close(childToParentPipe[0]);

      ssize_t bytesRead = read(parentToChildPipe[0], integerArray, sizeof(integerArray));
      if (bytesRead <= 0)
      {
        perror("Pipe read failed");
        exit(1);
      }

      sortIntegerArray(integerArray, ARRAY_LENGTH);

      write(childToParentPipe[1], integerArray, sizeof(integerArray));

      close(parentToChildPipe[0]);
      close(childToParentPipe[1]);
    }
    else
    {
      close(parentToChildPipe[0]);
      close(childToParentPipe[1]);

      printf("Before Sorting: ");
      printArray(integerArray, ARRAY_LENGTH);

      write(parentToChildPipe[1], integerArray, sizeof(integerArray));

      wait(NULL);

      read(childToParentPipe[0], integerArray, sizeof(integerArray));

      printf("After Sorting: ");
      printArray(integerArray, ARRAY_LENGTH);

      close(parentToChildPipe[1]);
      close(childToParentPipe[0]);
    }

    return 0;
  }
