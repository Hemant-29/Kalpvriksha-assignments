#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 30
#define CACHE_CAPACITY 1000
struct LRUCache *gCache = NULL;

typedef struct QueueNode
{
  int key;
  char value[STRING_LENGTH];
  struct QueueNode *next;
  struct QueueNode *previous;
} QueueNode;

typedef struct HashNode
{
  int key;
  QueueNode *queueAddress;
  struct HashNode *next;
} HashNode;

typedef struct LRUCache
{
  int capacity;
  int currentSize;
  QueueNode *front;
  QueueNode *rear;
  HashNode **hashMap;
} LRUCache;

int hashValue(LRUCache *cache, int key)
{
  return key % cache->capacity;
}

void insertToHashMap(LRUCache *cache, int key, QueueNode *value)
{
  int index = hashValue(cache, key);

  HashNode *newNode = malloc(sizeof(HashNode));
  newNode->key = key;
  newNode->queueAddress = value;
  newNode->next = cache->hashMap[index]; // Insert at head of bucket

  cache->hashMap[index] = newNode;
}

QueueNode *findInHashMap(LRUCache *cache, int key)
{
  int index = hashValue(cache, key);
  HashNode *current = cache->hashMap[index];

  while (current != NULL)
  {
    if (current->key == key)
      return current->queueAddress;
    current = current->next;
  }
  return NULL;
}

void deleteFromHashMap(LRUCache *cache, int key)
{
  int index = hashValue(cache, key);
  HashNode *current = cache->hashMap[index];
  HashNode *previous = NULL;

  while (current != NULL)
  {
    if (current->key == key)
    {
      if (previous == NULL)
      {
        cache->hashMap[index] = current->next;
      }
      else
      {
        previous->next = current->next;
      }
      free(current);
      return;
    }
    previous = current;
    current = current->next;
  }
}

void moveToHead(LRUCache *cache, QueueNode *node)
{
  QueueNode *head = cache->front;

  if (head == node)
  {
    return;
  }

  // Detach from current position
  if (node->previous)
  {
    node->previous->next = node->next;
  }
  if (node->next)
  {
    node->next->previous = node->previous;
  }

  // Update Rear if we moved the tail
  if (cache->rear == node)
  {
    cache->rear = node->previous;
    if (cache->rear)
    {
      cache->rear->next = NULL;
    }
  }

  // Attach to Front
  node->next = head;
  node->previous = NULL;

  if (cache->front)
  {
    cache->front->previous = node;
  }
  cache->front = node;
}

void deleteFromTail(LRUCache *cache)
{
  if (cache->rear == NULL)
    return;

  QueueNode *temp = cache->rear;

  if (cache->front == cache->rear)
  {
    cache->front = NULL;
    cache->rear = NULL;
  }
  else
  {
    cache->rear = cache->rear->previous;
    cache->rear->next = NULL;
  }

  free(temp);
  cache->currentSize--;
}

void put(LRUCache *cache, int key, char *value)
{
  // Check if Key exists
  QueueNode *existingNode = findInHashMap(cache, key);
  if (existingNode != NULL)
  {
    strncpy(existingNode->value, value, STRING_LENGTH - 1);
    existingNode->value[STRING_LENGTH - 1] = '\0';

    moveToHead(cache, existingNode);
    return;
  }

  // If Cache is Full, remove Tail (LRU)
  if (cache->currentSize >= cache->capacity)
  {
    int tailKey = cache->rear->key;
    deleteFromHashMap(cache, tailKey);
    deleteFromTail(cache);
  }

  QueueNode *newNode = malloc(sizeof(QueueNode));
  newNode->key = key;
  strncpy(newNode->value, value, STRING_LENGTH - 1);
  newNode->value[STRING_LENGTH - 1] = '\0';
  newNode->previous = NULL;
  newNode->next = cache->front;

  if (cache->front != NULL)
  {
    cache->front->previous = newNode;
  }
  cache->front = newNode;

  if (cache->rear == NULL)
  {
    cache->rear = newNode;
  }

  insertToHashMap(cache, key, newNode);
  cache->currentSize++;
}

char *get(LRUCache *cache, int key)
{
  QueueNode *node = findInHashMap(cache, key);

  if (node != NULL)
  {
    // Accessed. Move to MRU (Head)
    moveToHead(cache, node);
    return node->value;
  }
  return NULL;
}

void printQueue(LRUCache *cache)
{
  QueueNode *head = cache->front;
  printf("Queue Status: ");
  while (head != NULL)
  {
    printf("[%d: %s] -> ", head->key, head->value);
    head = head->next;
  }
  printf("NULL\n");
}

void createCache(int capacity)
{
  if (gCache != NULL)
  {
    printf("Cache already initialized\n");
    return;
  }

  gCache = malloc(sizeof(LRUCache));

  gCache->capacity = capacity;
  gCache->currentSize = 0;
  gCache->front = NULL;
  gCache->rear = NULL;

  gCache->hashMap = malloc(sizeof(HashNode *) * capacity);

  for (int i = 0; i < capacity; i++)
  {
    gCache->hashMap[i] = NULL;
  }
}

void processInput()
{
  printf("> ");

  char input[STRING_LENGTH];
  scanf("%[^\n]", input);
  getchar();

  char *command = strtok(input, " ");
  char *parameter1 = strtok(NULL, " ");
  char *parameter2 = strtok(NULL, " ");

  if (strcmp(command, "exit") == 0)
  {
    exit(0);
  }
  else if (strcmp(command, "get") == 0)
  {
    if (parameter1 == NULL)
    {
      printf("Invalid parameters\n");
      return;
    }
    int addressValue = atoi(parameter1);

    char *output = get(gCache, addressValue);
    if (output != NULL)
    {
      printf("%s\n", output);
    }
    else
    {
      printf("NULL\n");
    }
  }
  else if (strcmp(command, "put") == 0)
  {
    if (parameter1 == NULL)
    {
      printf("Invalid parameters\n");
      return;
    }
    if (parameter2 == NULL)
    {
      printf("Invalid parameters\n");
      return;
    }

    int addressValue = atoi(parameter1);
    if (addressValue < 0 || addressValue >= CACHE_CAPACITY)
    {
      printf("Invalid cache address\n");
      return;
    }
    char *cacheValue = parameter2;

    put(gCache, addressValue, cacheValue);
    printf("Added at %d successfully\n", addressValue);
  }
  else if (strcmp(command, "createCache") == 0)
  {
    if (parameter1 == NULL)
    {
      printf("Invalid parameters\n");
      return;
    }
    int value = atoi(parameter1);
    if (value >= CACHE_CAPACITY)
    {
      printf("Value too big\n");
      return;
    }
    createCache(value);
  }
  else
  {
    printf("%s is an Invalid command\n", command);
  }
}

int main()
{

  while (1)
  {
    processInput();
  }

  return 0;
}