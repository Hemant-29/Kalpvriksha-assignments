#include "vfs_library.h"

int freeBlockLength(FreeBlock *head)
{
  int length = 0;
  while (head != NULL)
  {
    length++;
    head = head->next;
  }
  return length;
}

void printFreeBlocks(FreeBlock *head)
{
  while (head != NULL)
  {
    printf("%d -> ", head->address);
    head = head->next;
  }
  printf("\n");
}

int initializeFreeBlocks(FreeBlock **head, FreeBlock **tail)
{
  if (head == NULL || tail == NULL)
  {
    return 0;
  }
  if (*head != NULL || *tail != NULL)
  {
    printf("Free Blocks array not empty!\n");
    return 0;
  }

  FreeBlock *previousNode = NULL;
  for (int index = 0; index < TOTAL_BLOCKS; index++)
  {
    FreeBlock *newNode = malloc(sizeof *newNode);
    if (newNode == NULL)
    {
      printf("Memory allocation failed!\n");
      return 0;
    }
    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->address = index;

    if (*head == NULL)
    {
      *head = newNode;
      *tail = newNode;
      newNode->previous = previousNode;
    }
    else
    {
      newNode->previous = previousNode;
      previousNode->next = newNode;
      *tail = newNode;
    }
    previousNode = newNode;
  }
  return 1;
}

int deleteFreeBlockFromHead(FreeBlock **head, FreeBlock **tail)
{
  if (head == NULL || tail == NULL)
  {
    return 0;
  }

  if (*head == NULL)
  {
    return 0;
  }

  if ((*head)->next == NULL)
  {
    free(*head);
    *head = NULL;
    *tail = NULL;
    return 1;
  }

  FreeBlock *nextNode = (*head)->next;
  nextNode->previous = NULL;
  free(*head);
  *head = nextNode;
  return 1;
}

int insertFreeBlockAtTail(FreeBlock **head, FreeBlock **tail, int address)
{
  if (head == NULL || tail == NULL)
  {
    return 0;
  }

  FreeBlock *newNode = malloc(sizeof *newNode);
  if (newNode == NULL)
  {
    printf("Memory allocation failed!\n");
    return 0;
  }
  newNode->next = NULL;
  newNode->previous = NULL;
  newNode->address = address;

  if (*head == NULL && *tail == NULL)
  {
    *head = newNode;
    *tail = newNode;
    return 1;
  }
  else if (*head == NULL || *tail == NULL)
  {
    free(newNode);
    printf("Invalid head and tail pointers!\n");
    return 0;
  }

  (*tail)->next = newNode;
  newNode->previous = *tail;
  *tail = newNode;
  return 1;
}
