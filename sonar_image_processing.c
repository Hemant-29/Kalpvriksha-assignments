#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BYTE_MASK 0xff
#define INPUT_UPPER_LIMIT 255

int packTo16Bits(int leftValue, int rightValue)
{
  // Packs the leftValue as Higher bits and the rightValue as the lower bits in a single 16 Bit integer
  if (leftValue < 0 || rightValue < 0)
  {
    printf("Error: Values can't be less than 0\n");
    return -1;
  }
  if (leftValue > INPUT_UPPER_LIMIT || rightValue > INPUT_UPPER_LIMIT)
  {
    printf("Error: Values can't be more than INPUT_UPPER_LIMIT\n");
    return -1;
  }
  int packedResult = ((int)leftValue << 8) | rightValue;

  return packedResult;
}

int unpackHigherByte(int combinedValue)
{
  // Here 0xff is the masking byte, which stands for 0000000011111111 in binary
  int higherByte = (combinedValue >> 8) & BYTE_MASK;
  // Returns the leftwards 8 bits
  return higherByte;
}

int unpackLowerByte(int combinedValue)
{
  int lowerByte = combinedValue & BYTE_MASK;
  return lowerByte;
}

void generateRandomMatrix(int const size, int (*matrixPointer)[size])
{

  srand(time(0)); // generate random seed once at the start

  for (int row = 0; row < size; row++)
  {
    for (int column = 0; column < size; column++)
    {
      int randomNumber = rand() % (INPUT_UPPER_LIMIT + 1); // for getting number in range 0 - 255
      *(*(matrixPointer + row) + column) = randomNumber;
    }
  }
}

void transposeMatrix(int const size, int (*matrixPointer)[size])
{
  for (int row = 0; row < size; row++)
  {
    for (int column = row + 1; column < size; column++)
    {
      int temporary = *(*(matrixPointer + row) + column);
      *(*(matrixPointer + row) + column) = *(*(matrixPointer + column) + row);
      *(*(matrixPointer + column) + row) = temporary;
    }
  }
}

void reverseMatrixRows(int const size, int (*matrixPointer)[size])
{
  for (int row = 0; row < size; row++)
  {
    int *columnStart = *(matrixPointer + row);
    int *columnEnd = *(matrixPointer + row) + size - 1;
    while (columnStart < columnEnd)
    {
      int temporary = *columnStart;
      *(columnStart) = *(columnEnd);
      *(columnEnd) = temporary;
      columnStart++;
      columnEnd--;
    }
  }
}

void rotateMatrixClockwise(int const size, int (*matrixPointer)[size])
{

  transposeMatrix(size, matrixPointer);
  reverseMatrixRows(size, matrixPointer);
}

void unpackMatrixValues(int const size, int (*matrixPointer)[size])
{
  for (int row = 0; row < size; row++)
  {
    for (int column = 0; column < size; column++)
    {
      *(*(matrixPointer + row) + column) = unpackHigherByte(*(*(matrixPointer + row) + column));
    }
  }
}

void applySmoothingFilter(int const size, int (*matrixPointer)[size])
{
  for (int row = 0; row < size; row++)
  {
    for (int column = 0; column < size; column++)
    {
      int *currentElement = (*(matrixPointer + row) + column);
      int sum = 0;
      int count = 0;

      for (int windowRow = row - 1; windowRow <= row + 1; windowRow++)
      {
        for (int windowColumn = column - 1; windowColumn <= column + 1; windowColumn++)
        {
          if (windowRow >= 0 && windowRow <= size - 1 && windowColumn >= 0 && windowColumn <= size - 1)
          {
            int neighborElement = *(*(matrixPointer + windowRow) + windowColumn);
            sum += unpackLowerByte(neighborElement);
            count++;
          }
        }
      }

      int average = sum / count;
      int packedValue = packTo16Bits(average, *currentElement);
      if (packedValue != -1)
      {
        *currentElement = packedValue;
      }
    }
  }
  unpackMatrixValues(size, matrixPointer);
}

void printMatrix(int const size, int (*matrixPointer)[size])
{
  for (int row = 0; row < size; row++)
  {
    for (int column = 0; column < size; column++)
    {
      int element = *(*(matrixPointer + row) + column);
      if (abs(element) < 10)
      {
        printf("00%d ", element);
      }
      else if (abs(element) < 100)
      {
        printf("0%d ", element);
      }
      else
      {
        printf("%d ", element);
      }
    }
    printf("\n");
  }
}

int main(void)
{
  int size = 0;
  printf("Enter the size of the matrix: ");
  scanf("%d", &size);

  if (size <= 0)
  {
    printf("Error: Invalid matrix size\n");
    return 1;
  }

  int matrix[size][size];

  generateRandomMatrix(size, matrix);
  printf("\nOriginal Randomly Generated Matrix: \n");
  printMatrix(size, matrix);

  rotateMatrixClockwise(size, matrix);
  printf("\nMatrix after 90 degree Clockwise Rotation: \n");
  printMatrix(size, matrix);

  printf("\nMatrix after Applying 3x3 Smoothing Filter: \n");
  applySmoothingFilter(size, matrix);
  printMatrix(size, matrix);

  return 0;
}