#include "vfs_library.h"

void printCircularLinkedList(FileNode *head)
{
  if (head == NULL)
  {
    return;
  }

  FileNode *current = head;
  do
  {
    if (current->isDirectory == 1)
    {
      printf("%s/\n", current->name);
    }
    else
    {
      printf("%s\n", current->name);
    }
    current = current->next;
  } while (current != head);
}

FileNode *insertFileNodeAtTail(FileNode *head, FileNode *parent, char name[], int isDirectory)
{
  FileNode *newNode = malloc(sizeof *newNode);
  if (newNode == NULL)
  {
    printf("Memory allocation failed!\n");
    return head;
  }
  newNode->next = NULL;
  newNode->previous = NULL;
  newNode->parent = parent;
  newNode->child = NULL;
  newNode->isDirectory = isDirectory;
  newNode->totalBlocks = 0;
  strncpy(newNode->name, name, MAX_NAME_SIZE - 1);

  if (head == NULL)
  {
    newNode->next = newNode;
    newNode->previous = newNode;
    return newNode;
  }

  FileNode *tail = head->previous;

  tail->next = newNode;
  newNode->previous = tail;
  newNode->next = head;
  head->previous = newNode;

  return head;
}

FileNode *insertFileNodeAsChild(FileNode *head, char name[], int isDirectory)
{

  if (head == NULL)
  {
    printf("Error: Invalid head, can't add child!\n");
    return NULL;
  }

  if (head->child == NULL)
  {
    FileNode *childHead = insertFileNodeAtTail(NULL, head, name, isDirectory);
    head->child = childHead;
  }
  else
  {
    FileNode *childHead = head->child;
    head->child = insertFileNodeAtTail(childHead, head, name, isDirectory);
  }

  return head;
}

int deleteFileNodeAtHead(FileNode *head, FreeBlock **freeBlockHead, FreeBlock **freeBlockTail, char virtualDisk[][BLOCK_SIZE])
{
  if (head == NULL)
  {
    return 0;
  }

  // Single Node case
  if (head->next == head)
  {

    if (head->child != NULL)
    {
      printf("can't delete Directory, delete sub-folder and files first!\n");
      return 0;
    }

    if (head->parent)
    {
      head->parent->child = NULL;
    }

    // free the blockPointers
    if (head->isDirectory != 1)
    {
      if (freeBlockHead == NULL || freeBlockTail == NULL || virtualDisk == NULL)
      {
        printf("Error: Invalid parameters!\n");
        return 0;
      }
      for (int index = 0; index < head->totalBlocks; index++)
      {
        int address = head->blockPointers[index];
        insertFreeBlockAtTail(freeBlockHead, freeBlockTail, address);

        // Free the virtual disk block
        strcpy(virtualDisk[address], "");
      }
    }

    free(head);
    head = NULL;
    return 1;
  }

  else
  {
    if (head->child != NULL)
    {
      printf("can't delete Directory, delete sub-folder and files first!\n");
      return 0;
    }

    if (head->parent)
    {
      if (head->parent->child == head)
      {
        head->parent->child = head->next;
      }
    }

    if (head->isDirectory != 1)
    {
      if (freeBlockHead == NULL || freeBlockTail == NULL || virtualDisk == NULL)
      {
        printf("Error: Invalid parameters!\n");
        return 0;
      }
      for (int index = 0; index < head->totalBlocks; index++)
      {
        int address = head->blockPointers[index];
        insertFreeBlockAtTail(freeBlockHead, freeBlockTail, address);

        strcpy(virtualDisk[address], "");
      }
    }

    FileNode *previousNode = head->previous;
    FileNode *nextNode = head->next;

    previousNode->next = nextNode;
    nextNode->previous = previousNode;

    free(head);
    head = NULL;
    return 1;
  }
  return 0;
}

FileNode *searchFileInDirectory(FileNode *currentDirectory, char *fileName)
{
  if (currentDirectory == NULL)
  {
    return NULL;
  }
  if (currentDirectory->isDirectory == 0 || currentDirectory->child == NULL)
  {
    return NULL;
  }

  FileNode *childHead = currentDirectory->child;
  FileNode *temp = childHead;

  do
  {
    if (strcmp(temp->name, fileName) == 0)
    {
      return temp;
    }
    temp = temp->next;
  } while (temp != childHead);

  return NULL;
}