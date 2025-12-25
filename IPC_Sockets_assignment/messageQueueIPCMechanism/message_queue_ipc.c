#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "../utils.h"

struct MessagePacket
{
  long messageType;
  int integerArray[ARRAY_LENGTH];
};

int setupMessageQueue(const char *filename, int projectId)
{
  FILE *filePointer = fopen(filename, "a");
  if (filePointer == NULL)
  {
    perror("File creation failed");
    exit(1);
  }
  fclose(filePointer);

  key_t key = ftok(filename, projectId);
  if (key == -1)
  {
    perror("ftok failed");
    exit(1);
  }

  int queueId = msgget(key, 0666 | IPC_CREAT);
  if (queueId == -1)
  {
    perror("msgget failed");
    exit(1);
  }
  return queueId;
}

void sendMessage(int queueId, struct MessagePacket *packet)
{
  // We send only the size of the array, not the full struct size
  if (msgsnd(queueId, packet, sizeof(packet->integerArray), 0) == -1)
  {
    perror("msgsnd failed");
    exit(1);
  }
}

void receiveMessage(int queueId, struct MessagePacket *packet, long messageType)
{
  // 4th arg (messageType) lets us filter which message to read
  if (msgrcv(queueId, packet, sizeof(packet->integerArray), messageType, 0) == -1)
  {
    perror("msgrcv failed");
    exit(1);
  }
}

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
  int messageQueueId = setupMessageQueue("msgqueuefile", 65);

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
    receiveMessage(messageQueueId, &message, 1);

    sortIntegerArray(message.integerArray, ARRAY_LENGTH);

    sendMessage(messageQueueId, &message);
  }
  else
  {
    printf("Before Sorting: ");
    printArray(integerArray, ARRAY_LENGTH);

    // Prepare data
    for (int copyIndex = 0; copyIndex < ARRAY_LENGTH; copyIndex++)
    {
      message.integerArray[copyIndex] = integerArray[copyIndex];
    }

    // Send to child
    sendMessage(messageQueueId, &message);

    wait(NULL);

    receiveMessage(messageQueueId, &message, 1);

    printf("After Sorting: ");
    printArray(message.integerArray, ARRAY_LENGTH);

    // Cleanup
    msgctl(messageQueueId, IPC_RMID, NULL);
  }

  return 0;
}
