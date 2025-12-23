#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define ARRAY_LENGTH 5

struct MessagePacket
{
  long messageType;
  int integerArray[ARRAY_LENGTH];
};

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
  // Create a new file
  FILE *filePointer = fopen("msgqueuefile", "w");
  if (filePointer == NULL)
  {
    perror("file creation failed");
    exit(1);
  }
  fclose(filePointer);

  key_t messageQueueKey = ftok("msgqueuefile", 65);
  if (messageQueueKey == -1)
  {
    perror("ftok failed");
    exit(1);
  }

  int messageQueueId = msgget(messageQueueKey, 0666 | IPC_CREAT);
  if (messageQueueId == -1)
  {
    perror("msgget failed");
    exit(1);
  }

  struct MessagePacket message;
  message.messageType = 1;

  int integerArray[ARRAY_LENGTH] = {4, 1, 6, 9, 2};

  pid_t processId = fork();
  if (processId < 0)
  {
    perror("fork failed");
    exit(1);
  }

  if (processId == 0)
  {
    msgrcv(messageQueueId, &message, sizeof(message.integerArray), 1, 0);

    sortIntegerArray(message.integerArray, ARRAY_LENGTH);

    msgsnd(messageQueueId, &message, sizeof(message.integerArray), 0);
  }
  else
  {
    printf("Before Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);

    for (int copyIndex = 0; copyIndex < ARRAY_LENGTH; copyIndex++)
    {
      message.integerArray[copyIndex] = integerArray[copyIndex];
    }

    msgsnd(messageQueueId, &message, sizeof(message.integerArray), 0);

    wait(NULL);

    msgrcv(messageQueueId, &message, sizeof(message.integerArray), 1, 0);

    printf("After Sorting: ");
    printArray(message.integerArray, ARRAY_LENGTH);

    msgctl(messageQueueId, IPC_RMID, NULL);
  }

  return 0;
}
