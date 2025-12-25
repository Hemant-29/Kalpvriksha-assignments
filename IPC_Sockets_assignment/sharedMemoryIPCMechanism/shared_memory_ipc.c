#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "../utils.h"

int getSharedMemoryId(const char *filename, int size)
{
  // Ensure file exists for ftok
  FILE *fp = fopen(filename, "a");
  if (fp == NULL)
  {
    perror("File open failed");
    exit(1);
  }
  fclose(fp);

  // Generate Key
  key_t key = ftok(filename, 65);
  if (key == -1)
  {
    perror("ftok failed");
    exit(1);
  }

  // Get ID
  int shmId = shmget(key, size, 0666 | IPC_CREAT);
  if (shmId == -1)
  {
    perror("shmget failed");
    exit(1);
  }

  return shmId;
}

int *attachSharedMemory(int shmId)
{
  int *array = (int *)shmat(shmId, NULL, 0);
  if (array == (void *)-1)
  {
    perror("shmat failed");
    exit(1);
  }
  return array;
}

void detachAndRemoveMemory(int *array, int shmId)
{
  // Detach from process
  if (shmdt(array) == -1)
  {
    perror("shmdt failed");
  }
  // Remove from OS
  if (shmctl(shmId, IPC_RMID, NULL) == -1)
  {
    perror("shmctl failed");
  }
}

int main()
{
  int sharedMemoryId = getSharedMemoryId("sharedmemoryfile", sizeof(int) * ARRAY_LENGTH);
  int *sharedIntegerArray = attachSharedMemory(sharedMemoryId);

  pid_t processId = fork();

  if (processId < 0)
  {
    perror("fork failed");
    exit(1);
  }

  if (processId == 0)
  {
    // Wait specifically for data to be written by parent
    sleep(1);

    sortIntegerArray(sharedIntegerArray, ARRAY_LENGTH);

    // Child should detach, but NOT remove
    shmdt(sharedIntegerArray);
  }
  else
  {
    int initialValues[ARRAY_LENGTH] = {8, 3, 1, 7, 4};

    // Write data to shared memory
    for (int copyIndex = 0; copyIndex < ARRAY_LENGTH; copyIndex++)
    {
      sharedIntegerArray[copyIndex] = initialValues[copyIndex];
    }

    printf("Before Sorting: ");
    printArray(sharedIntegerArray, ARRAY_LENGTH);

    wait(NULL); // Wait for child to finish sorting

    printf("After Sorting: ");
    printArray(sharedIntegerArray, ARRAY_LENGTH);

    // Cleanup
    detachAndRemoveMemory(sharedIntegerArray, sharedMemoryId);
  }

  return 0;
}