#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
  key_t sharedMemoryKey = ftok("sharedmemoryfile", 65);
  if (sharedMemoryKey == -1)
  {
    perror("ftok failed");
    exit(1);
  }

  int sharedMemoryId = shmget(sharedMemoryKey, sizeof(int) * ARRAY_LENGTH, 0666 | IPC_CREAT);
  if (sharedMemoryId == -1)
  {
    perror("shmget failed");
    exit(1);
  }

  int *sharedIntegerArray = (int *)shmat(sharedMemoryId, NULL, 0);
  if (sharedIntegerArray == (void *)-1)
  {
    perror("shmat failed");
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
    sleep(1);
    sortIntegerArray(sharedIntegerArray, ARRAY_LENGTH);
  }
  else
  {
    int initialValues[ARRAY_LENGTH] = {8, 3, 1, 7, 4};

    for (int copyIndex = 0; copyIndex < ARRAY_LENGTH; copyIndex++)
    {
      sharedIntegerArray[copyIndex] = initialValues[copyIndex];
    }

    printf("Before Sorting: ");
    printArray(sharedIntegerArray, ARRAY_LENGTH);

    wait(NULL);

    printf("After Sorting: ");
    printArray(sharedIntegerArray, ARRAY_LENGTH);

    shmdt(sharedIntegerArray);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
  }

  return 0;
}
