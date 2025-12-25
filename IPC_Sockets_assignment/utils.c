// utils.c
#include <stdio.h>
#include "utils.h"

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