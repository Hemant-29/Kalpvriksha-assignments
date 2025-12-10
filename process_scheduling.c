#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NAME_LIMIT 100
#define PROCESS_LIMIT 100

enum ProcessState
{
  READY,
  RUNNING,
  WAITING,
  TERMINATED
};

typedef struct PCB
{
  int id;
  char processName[NAME_LIMIT];
  int cpuBurstTime;
  int ioStartTime;
  int ioDuration;
  enum ProcessState state;

  int cpuTimeUsed;
  int ioTimeRemaining;
  int completionTime;

} PCB;

typedef struct QueueNode
{
  PCB *processPointer;
  struct QueueNode *next;
  struct QueueNode *prev;
} QueueNode;

typedef struct Queue
{
  QueueNode *head;
  QueueNode *tail;
} Queue;

typedef struct HashNode
{
  int key;
  PCB *value;
  struct HashNode *next;
} HashNode;

typedef struct Hash
{
  HashNode *array[PROCESS_LIMIT];
} Hash;

typedef struct killNode
{
  int pid;
  int killTime;
  struct killNode *next;
} killNode;

PCB *createProcess(int id, const char *name, int cpuBurst, int ioStart, int ioDuration)
{
  PCB *process = malloc(sizeof(PCB));
  if (process == NULL)
  {
    printf("Memory allocation failed\n");
    return NULL;
  }

  process->id = id;
  if (name == NULL)
  {
    return NULL;
  }
  strncpy(process->processName, name, NAME_LIMIT);
  process->processName[NAME_LIMIT - 1] = '\0';
  process->cpuBurstTime = cpuBurst;
  process->ioStartTime = ioStart;
  process->ioDuration = ioDuration;
  process->state = READY;
  process->cpuTimeUsed = 0;
  process->ioTimeRemaining = ioDuration;
  process->completionTime = 0;

  return process;
}

void enQueue(Queue *que, PCB *processPointer)
{
  if (que == NULL)
  {
    printf("Invalid Queue\n");
    return;
  }

  QueueNode *newNode = malloc(sizeof(QueueNode));
  if (newNode == NULL)
  {
    printf("Memory allocation failed\n");
    return;
  }

  newNode->next = NULL;
  newNode->prev = NULL;
  newNode->processPointer = processPointer;

  // Edge Case: No node in the queue
  if (que->tail == NULL || que->head == NULL)
  {
    que->head = newNode;
    que->tail = newNode;
    return;
  }

  que->tail->next = newNode;
  newNode->prev = que->tail;
  que->tail = newNode;
}

PCB *deQueue(Queue *que)
{
  if (que == NULL)
  {
    printf("Invalid queue\n");
    return NULL;
  }

  if (que->head == NULL)
  {
    printf("Queue already empty\n");
    return NULL;
  }

  // Single Node case
  if (que->head == que->tail)
  {
    PCB *processPointer = que->head->processPointer;
    free(que->head);
    que->head = NULL;
    que->tail = NULL;
    return processPointer;
  }

  PCB *processPointer = que->head->processPointer;

  QueueNode *nextNode = que->head->next;
  nextNode->prev = NULL;
  free(que->head);
  que->head = nextNode;

  return processPointer;
}

PCB *removeQueueNode(Queue *que, QueueNode *node)
{
  if (node == NULL)
  {
    printf("Invalid node\n");
    return NULL;
  }

  PCB *process = node->processPointer;

  QueueNode *previousNode = node->prev;
  QueueNode *nextNode = node->next;

  if (previousNode != NULL)
  {
    previousNode->next = nextNode;
  }
  else
  {
    que->head = nextNode;
  }

  if (nextNode != NULL)
  {
    nextNode->prev = previousNode;
  }
  else
  {
    que->tail = previousNode;
  }

  free(node);
  return process;
}

PCB *removeProcessFromQueue(Queue *que, PCB *process)
{
  if (que == NULL || process == NULL)
  {
    printf("Invalid process\n");
    return NULL;
  }

  QueueNode *node = que->head;
  while (node != NULL && node->processPointer != process)
  {
    node = node->next;
  }

  if (node == NULL)
  {
    return NULL;
  }
  QueueNode *previousNode = node->prev;
  QueueNode *nextNode = node->next;

  if (previousNode != NULL)
  {
    previousNode->next = nextNode;
  }
  else
  {
    que->head = nextNode;
  }
  if (nextNode != NULL)
  {
    nextNode->prev = previousNode;
  }
  else
  {
    que->tail = previousNode;
  }

  free(node);
  return process;
}

int hashingFunction(int key, int size)
{
  return (key % size);
}

void initializeHash(Hash *hashMap)
{
  for (int index = 0; index < PROCESS_LIMIT; index++)
  {
    hashMap->array[index] = NULL;
  }
}

void insertInHash(int pid, Hash *map, PCB *process)
{
  if (map == NULL)
  {
    printf("Invalid hash Map\n");
    return;
  }
  if (process == NULL)
  {
    printf("Invalid process\n");
    return;
  }

  int hashKey = hashingFunction(pid, PROCESS_LIMIT);

  HashNode *newNode = malloc(sizeof(HashNode));
  if (newNode == NULL)
  {
    printf("Memory allocation failed\n");
    return;
  }
  newNode->next = NULL;
  newNode->key = pid;
  newNode->value = process;

  HashNode *head = map->array[hashKey];
  while (head != NULL)
  {
    if (head->key == pid)
    {
      printf("Updating value at pid %d\n", head->key);
      head->value = process;
      return;
    }
    head = head->next;
  }

  newNode->next = map->array[hashKey];
  map->array[hashKey] = newNode;
}

PCB *findInHash(int pid, Hash *map)
{
  if (map == NULL)
  {
    printf("Invalid hash Map\n");
    return NULL;
  }

  int hashKey = hashingFunction(pid, PROCESS_LIMIT);

  HashNode *head = map->array[hashKey];
  while (head != NULL)
  {
    if (head->key == pid)
    {

      return head->value;
    }
    head = head->next;
  }

  // Not found in the hash map
  return NULL;
}

void addToKillLinkedList(killNode **killHead, int pid, int killTime)
{
  killNode *newNode = malloc(sizeof(killNode));
  newNode->next = NULL;
  newNode->pid = pid;
  newNode->killTime = killTime;

  if (*killHead == NULL)
  {
    *killHead = newNode;
    return;
  }

  killNode *temp = *killHead;
  killNode *previous = NULL;

  while (temp != NULL && temp->killTime < killTime)
  {
    previous = temp;
    temp = temp->next;
  }

  if (previous == NULL)
  {
    newNode->next = *killHead;
    *killHead = newNode;
  }
  else
  {
    newNode->next = temp;
    previous->next = newNode;
  }
}

void deleteFromKillLinkedList(killNode **killHead)
{
  if (*killHead == NULL)
  {
    return;
  }

  killNode *headPrev = *killHead;
  *killHead = (*killHead)->next;
  headPrev->next = NULL;
  free(headPrev);
}

void processReadyQueue(Queue *ready, Queue *waiting, Queue *terminated, int clock)
{
  if (ready->head == NULL || ready->head->processPointer == NULL)
  {
    return;
  }

  PCB *headProcess = ready->head->processPointer;

  headProcess->state = RUNNING;

  headProcess->cpuTimeUsed++;

  // If process has completed
  if (headProcess->cpuTimeUsed >= headProcess->cpuBurstTime)
  {
    PCB *process = deQueue(ready);

    enQueue(terminated, process);
    process->state = TERMINATED;

    // set clock + 1, for setting completion time to end of this tick
    process->completionTime = clock + 1;

    return;
  }

  // Start the I/O:
  if (headProcess->cpuTimeUsed == headProcess->ioStartTime)
  {

    // Set ioDuration + 1, because the transition from ready to waiting will be done in the next second
    headProcess->ioTimeRemaining = headProcess->ioDuration + 1;
    headProcess->state = WAITING;

    PCB *process = deQueue(ready);
    enQueue(waiting, process);
  }
}

void processWaitingQueue(Queue *ready, Queue *waiting)
{
  QueueNode *temp;
  temp = waiting->head;
  while (temp != NULL)
  {
    QueueNode *nextNode = temp->next;

    if (temp->processPointer)
    {
      temp->processPointer->ioTimeRemaining--;

      // Condition for IO complete
      if (temp->processPointer->ioTimeRemaining <= 0)
      {
        PCB *process = removeQueueNode(waiting, temp);

        enQueue(ready, process);
        process->state = READY;
      }
    }

    temp = nextNode;
  }
}

void handleKillEvent(Hash *map, Queue *ready, Queue *waiting, Queue *terminated, killNode **killHead, int clock)
{
  if (killHead == NULL || (*killHead) == NULL)
  {
    return;
  }

  while ((*killHead)->killTime == clock)
  {
    int pidToKill = (*killHead)->pid;

    deleteFromKillLinkedList(killHead);

    PCB *process = findInHash(pidToKill, map);

    if (process != NULL)
    {
      process->state = TERMINATED;

      removeProcessFromQueue(ready, process);
      removeProcessFromQueue(waiting, process);

      enQueue(terminated, process);

      process->completionTime = clock;
    }
  }
}

void printStats(Queue *terminated)
{
  if (terminated == NULL)
  {
    printf("No process in the terminated queue\n");
    return;
  }

  printf("%-8s %-12s %-8s %-8s %-12s %-14s %-14s\n",
         "PID", "Name", "CPU", "IO", "Status", "Turn around", "Waiting time");

  QueueNode *temp = terminated->head;
  while (temp != NULL)
  {
    PCB *process = temp->processPointer;

    int ioDone = process->ioDuration - process->ioTimeRemaining;

    // CASE: Process NOT executed completely
    if (process->cpuTimeUsed < process->cpuBurstTime)
    {
      printf("%-8d %-12s %-8d %-8d Killed at %-4d %-14s %-14s\n",
             process->id,
             process->processName,
             process->cpuTimeUsed,
             ioDone,
             process->completionTime,
             "_",
             "_");

      temp = temp->next;
      continue;
    }

    int turnAroundTime = process->completionTime;
    int waitingTime = turnAroundTime - process->cpuBurstTime;

    printf("%-8d %-12s %-8d %-8d %-14s %-14d %-14d\n",
           process->id,
           process->processName,
           process->cpuTimeUsed,
           ioDone,
           "OK",
           turnAroundTime,
           waitingTime);

    temp = temp->next;
  }
}

void simulateScheduling(Hash *map, Queue *ready, Queue *waiting, Queue *terminated, killNode **killHead)
{
  int clock = 0;

  while (ready->head != NULL || waiting->head != NULL || *killHead != NULL)
  {

    if (ready->head == NULL && waiting->head == NULL)
    {
      break;
    }

    handleKillEvent(map, ready, waiting, terminated, killHead, clock);

    processWaitingQueue(ready, waiting);

    processReadyQueue(ready, waiting, terminated, clock);

    sleep(1);
    clock++;
  }
}

void addProcess(Hash *map, Queue *ready, int pid, char *name, int cpuBurst, int ioStart, int ioDuration)
{
  if (findInHash(pid, map) != NULL)
  {
    printf("Process with PID %d already exists. Skipping.\n", pid);
    return;
  }

  PCB *process = createProcess(pid, name, cpuBurst, ioStart, ioDuration);
  if (process == NULL)
  {
    return;
  }

  insertInHash(pid, map, process);

  enQueue(ready, process);
}

void handleInput(killNode **killHead, Hash *map, Queue *readyQueue)
{
  int processCount;

  printf("Enter the no. of processes: ");
  if (scanf("%d", &processCount) != 1 || processCount <= 0)
  {
    printf("Invalid input. Enter a positive integer ");
    return;
  }

  for (int i = 0; i < processCount; i++)
  {
    int pid, cpuBurst, ioStart, ioDuration;
    char processName[NAME_LIMIT];

    printf("Enter process %d details (Name PID CPU_Burst IO_Start IO_Duration): \n", i + 1);

    if (scanf("%s %d %d %d %d", processName, &pid, &cpuBurst, &ioStart, &ioDuration) != 5)
    {
      printf("Invalid input format.\n");
      return;
    }

    if (cpuBurst <= 0 || ioStart < 0 || ioDuration < 0)
    {
      printf("Invalid time values (Burst must be > 0).\n");
      return;
    }

    addProcess(map, readyQueue, pid, processName, cpuBurst, ioStart, ioDuration);
  }

  int killProcesses;
  printf("Enter the no. of processes to kill: ");
  if (scanf("%d", &killProcesses) != 1 || killProcesses < 0)
  {
    printf("Invalid kill count.\n");
    return;
  }
  for (int i = 0; i < killProcesses; i++)
  {
    int pid;
    int killTime;

    printf("Enter process %d details: \n", i);

    if (scanf("%d %d", &pid, &killTime) != 2)
    {
      printf("Invalid kill input format.\n");
      return;
    }

    // Validate PID existence
    if (findInHash(pid, map) == NULL)
    {
      printf("PID %d not found\n", pid);
      return;
    }

    if (killTime < 0)
    {
      printf("Kill time cannot be negative\n");
      return;
    }

    addToKillLinkedList(killHead, pid, killTime);
  }
}

void freeSystemResources(Hash *map, Queue *ready, Queue *waiting, Queue *terminated, killNode **killHead)
{
  // Free all PCBs and HashNodes via the Hash Map
  for (int i = 0; i < PROCESS_LIMIT; i++)
  {
    HashNode *current = map->array[i];
    while (current != NULL)
    {
      HashNode *temp = current;

      free(current->value);

      current = current->next;
      free(temp);
    }
  }

  // Free Queue Nodes (Ready, Waiting, Terminated)
  Queue *queues[] = {ready, waiting, terminated};
  for (int i = 0; i < 3; i++)
  {
    QueueNode *node = queues[i]->head;
    while (node != NULL)
    {
      QueueNode *temp = node;
      node = node->next;
      free(temp);
    }
  }

  // Free remaining Kill Nodes
  killNode *kNode = *killHead;
  while (kNode != NULL)
  {
    killNode *temp = kNode;
    kNode = kNode->next;
    free(temp);
  }
}

int main()
{
  // Initialize hashMap
  Hash hashMap;
  initializeHash(&hashMap);

  // initialize all the Queues
  Queue readyQueue;
  readyQueue.head = NULL;
  readyQueue.tail = NULL;

  Queue waitingQueue;
  waitingQueue.head = NULL;
  waitingQueue.tail = NULL;

  Queue terminatedQueue;
  terminatedQueue.head = NULL;
  terminatedQueue.tail = NULL;

  // Making a killed processes Linked List
  killNode *killHead = NULL;

  handleInput(&killHead, &hashMap, &readyQueue);

  simulateScheduling(&hashMap, &readyQueue, &waitingQueue, &terminatedQueue, &killHead);

  printStats(&terminatedQueue);

  freeSystemResources(&hashMap, &readyQueue, &waitingQueue, &terminatedQueue, &killHead);

  return 0;
}