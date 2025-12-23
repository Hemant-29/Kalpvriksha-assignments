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
  int integerArray[] = {5, 2, 9, 1, 3};
  FILE *dataFilePointer;
  pid_t processId = fork();
  if (processId < 0)
  {
    perror("fork failed");
    exit(1);
  }

  if (processId == 0)
  {
    sleep(1);

    dataFilePointer = fopen("data.txt", "r");
    if (dataFilePointer == NULL)
    {
      perror("File open failed");
      exit(1);
    }
    fread(integerArray, sizeof(int), ARRAY_LENGTH, dataFilePointer);
    fclose(dataFilePointer);

    sortIntegerArray(integerArray, ARRAY_LENGTH);

    dataFilePointer = fopen("data.txt", "w");
    if (dataFilePointer == NULL)
    {
      perror("File open failed");
      exit(1);
    }
    fwrite(integerArray, sizeof(int), ARRAY_LENGTH, dataFilePointer);
    fclose(dataFilePointer);
  }
  else
  {
    dataFilePointer = fopen("data.txt", "w");
    if (dataFilePointer == NULL)
    {
      perror("File open failed");
      exit(1);
    }
    fwrite(integerArray, sizeof(int), ARRAY_LENGTH, dataFilePointer);
    fclose(dataFilePointer);

    printf("Before Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);

    wait(NULL);

    dataFilePointer = fopen("data.txt", "r");
    if (dataFilePointer == NULL)
    {
      perror("File open failed");
      exit(1);
    }
    fread(integerArray, sizeof(int), ARRAY_LENGTH, dataFilePointer);
    fclose(dataFilePointer);

    printf("After Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);
  }
  return 0;
}
