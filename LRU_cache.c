#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_LENGTH 30
#define CACHE_CAPACITY 1000

typedef struct QueueNode
{
  int key;
  char value[STRING_LENGTH];
  struct QueueNode *next;
  struct QueueNode *prev;
} QueueNode;

typedef struct HashNode
{
  int key;
  QueueNode *queueAddress;
  struct HashNode *next;
} HashNode;

int hashValue(int key, int size)
{
  return key % size;
}

void insertToHashMap(HashNode **hashMap, int key, QueueNode *value)
{
  int hashIndex = hashValue(key, CACHE_CAPACITY);

  HashNode *currentNode = hashMap[hashIndex];
  while (currentNode != NULL)
  {
    if (currentNode->key == key)
    {
      // key found, update its value in the HashMap
      currentNode->queueAddress = value;
      return;
    }
    currentNode = currentNode->next;
  }

  // Insert at the head, if value not found in the hashMap
  HashNode *head = hashMap[hashIndex];

  HashNode *newNode = malloc(sizeof(HashNode));
  newNode->key = key;
  newNode->queueAddress = value;
  newNode->next = NULL;

  if (head == NULL)
  {
    hashMap[hashIndex] = newNode;
  }
  else
  {
    newNode->next = head;
    hashMap[hashIndex] = newNode;
  }
}

QueueNode *findInHashMap(HashNode **hashMap, int key)
{
  int hashIndex = hashValue(key, CACHE_CAPACITY);

  HashNode *currentNode = hashMap[hashIndex];
  while (currentNode != NULL)
  {
    if (currentNode->key == key)
    {
      printf("Found key %d in hashMap\n", key);
      return currentNode->queueAddress;
    }
    currentNode = currentNode->next;
  }
  printf("Key %d NOT found in hashMap\n", key);
  return NULL;
}

void deleteFromHashMap(HashNode **hashMap, int key)
{
  int hashIndex = hashValue(key, CACHE_CAPACITY);

  HashNode *currentNode = hashMap[hashIndex];
  HashNode *previousNode = NULL;

  while (currentNode != NULL)
  {
    if (currentNode->key == key)
    {
      printf("Deleted key %d from hashMap\n", key);
      if (previousNode == NULL)
      {
        hashMap[hashIndex] = currentNode->next;
        free(currentNode);
      }
      else
      {
        previousNode->next = currentNode->next;
        free(currentNode);
      }
      break;
    }
    previousNode = currentNode;
    currentNode = currentNode->next;
  }
}

int getLinkedListSize(QueueNode *head)
{
  int size = 0;
  while (head != NULL)
  {
    size++;
    head = head->next;
  }
  return size;
}

void deleteFromQueue(QueueNode **head, QueueNode **tail, int *size)
{
  if (*tail == NULL)
  {
    return;
  }

  if (*head == *tail)
  {
    free(*head);
    *head = NULL;
    *tail = NULL;
    --*size;
    return;
  }

  QueueNode *previousTail = *tail;
  *tail = (*tail)->prev;
  (*tail)->next = NULL;
  free(previousTail);
  --*size;
}

void addToQueue(QueueNode **head, QueueNode **tail, int key, char *value, HashNode **hashMap, int *size)
{
  // if key is present in the Cache Queue
  QueueNode *address = findInHashMap(hashMap, key);
  if (address != NULL)
  {
    strncpy(address->value, value, STRING_LENGTH - 1);
    address->value[STRING_LENGTH - 1] = '\0';
    return;
  }

  // If queue is Empty
  if (*head == NULL)
  {
    QueueNode *newNode = malloc(1 * sizeof(QueueNode));
    newNode->key = key;
    strncpy(newNode->value, value, STRING_LENGTH - 1);
    newNode->value[STRING_LENGTH - 1] = '\0';
    newNode->prev = NULL;
    newNode->next = NULL;

    *head = newNode;
    *tail = newNode;

    insertToHashMap(hashMap, key, *head);
    ++*size;
    return;
  }

  // Capacity check for the queue
  if (*size >= CACHE_CAPACITY)
  {
    int tailKey = (*tail)->key;
    deleteFromHashMap(hashMap, tailKey);

    deleteFromQueue(head, tail, size);
  }

  QueueNode *newHead = malloc(1 * sizeof(QueueNode));
  newHead->next = NULL;
  newHead->prev = NULL;
  newHead->key = key;
  strncpy(newHead->value, value, STRING_LENGTH - 1);
  newHead->value[STRING_LENGTH - 1] = '\0';

  newHead->next = *head;
  (*head)->prev = newHead;

  *head = newHead;

  // Then update the key-value in the hashmap
  insertToHashMap(hashMap, key, *head);

  ++*size;
}

void printQueue(QueueNode *head)
{
  while (head != NULL)
  {
    printf("%d: %s -> ", head->key, head->value);
    head = head->next;
  }
  printf("\n");
}

int main()
{
  // Initialize LRU Cache
  QueueNode *front = NULL;
  QueueNode *rear = NULL;
  int size = 0;
  HashNode **hashMap = malloc(CACHE_CAPACITY * sizeof(HashNode *));
  for (int item = 0; item < CACHE_CAPACITY; item++)
  {
    hashMap[item] = NULL;
  }

  addToQueue(&front, &rear, 102, "abcd", hashMap, &size);
  addToQueue(&front, &rear, 1102, "dln", hashMap, &size);
  printQueue(front);
  return 0;
}