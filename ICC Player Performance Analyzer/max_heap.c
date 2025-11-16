#include "ICC_header.h"

void swapNodes(heapNode *first, heapNode *second)
{
  if (first == NULL || second == NULL)
  {
    printf("Can't swap values");
    return;
  }
  heapNode temp = *first;
  *first = *second;
  *second = temp;
}

void insertInMaxHeap(heapNode *heap, int *size, int capacity, heapNode node)
{
  if (*size >= capacity)
  {
    printf("Heap capacity full\n");
    return;
  }

  int currentIndex = *size + 1;
  int parentIndex = currentIndex / 2;

  heap[currentIndex] = node;
  (*size)++;

  while (parentIndex > 0 && heap[currentIndex].data->PerformanceIndex > heap[parentIndex].data->PerformanceIndex)
  {
    swapNodes(&heap[currentIndex], &heap[parentIndex]);

    currentIndex = parentIndex;
    parentIndex = currentIndex / 2;
  }
}

void deleteFromMaxHeap(heapNode *heap, int *size)
{
  if (heap == NULL || *size <= 0)
  {
    return;
  }

  heap[1] = heap[*size];

  (*size)--;

  // Moving down the top element to its correct place
  int current = 1;
  int leftChild = 2 * current;
  int rightChild = 2 * current + 1;
  int largest = current;
  while (current <= *size)
  {
    if (leftChild <= *size && heap[largest].data->PerformanceIndex < heap[leftChild].data->PerformanceIndex)
    {
      largest = leftChild;
    }
    if (rightChild <= *size && heap[largest].data->PerformanceIndex < heap[rightChild].data->PerformanceIndex)
    {
      largest = rightChild;
    }

    if (largest != current)
    {
      swapNodes(&heap[current], &heap[largest]);
      current = largest;
    }
    else
    {
      break;
    }
    leftChild = 2 * current;
    rightChild = 2 * current + 1;
  }
}

void printMergedSortedPlayers(int *arrayLengths, int totalArrays, PlayerNode **arrays)
{
  if (totalArrays <= 0 || arrays == NULL)
  {
    printf("Invalid input to sortKArrays\n");
    return;
  }

  // Allocated totalArrays+1 because heap is 1-indexed in insertInMinHeap.
  int heapCapacity = totalArrays;
  int heapSize = 0;
  heapNode *heap = calloc(heapCapacity + 1, sizeof(heapNode));
  if (heap == NULL)
  {
    printf("Memory allocation failed!\n");
    return;
  }

  // Insert first elements of all arrays into maxHeap
  for (int arrayIndex = 0; arrayIndex < totalArrays; arrayIndex++)
  {
    if (arrays[arrayIndex] == NULL || arrayLengths[arrayIndex] <= 0)
    {
      continue;
    }

    heapNode element = {&arrays[arrayIndex][0], arrayIndex, 0};
    insertInMaxHeap(heap, &heapSize, heapCapacity, element);
  }

  // Extract-maximum and insert next element from that array until heap is 0
  while (heapSize > 0)
  {
    displayPlayer(heap[1].data);

    int arrayIndex = heap[1].array;
    int valueIndex = heap[1].index + 1;

    deleteFromMaxHeap(heap, &heapSize);

    if (arrays[arrayIndex] != NULL && valueIndex < arrayLengths[arrayIndex])
    {
      heapNode element = {&arrays[arrayIndex][valueIndex], arrayIndex, valueIndex};
      insertInMaxHeap(heap, &heapSize, heapCapacity, element);
    }
  }

  free(heap);
}