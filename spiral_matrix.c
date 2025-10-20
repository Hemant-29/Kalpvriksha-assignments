#include <stdio.h>
#include <stdlib.h>

void generateSpiralMatrix(int const size, int *matrix)
{
  if (matrix == NULL)
  {
    return;
  }
  int currentValue = 1;
  int row = 0;
  int column = 0;
  for (int padding = 1; padding < size; padding++)
  {
    while (column < size - padding)
    {
      *(matrix + row * size + column) = currentValue;
      column++;
      currentValue++;
    }
    while (row < size - padding)
    {
      *(matrix + row * size + column) = currentValue;
      row++;
      currentValue++;
    }
    while (column >= 0 + padding)
    {
      *(matrix + row * size + column) = currentValue;
      column--;
      currentValue++;
    }
    while (row >= 0 + padding + 1)
    {
      *(matrix + row * size + column) = currentValue;
      row--;
      currentValue++;
    }
  }
  *(matrix + row * size + column) = currentValue;
}

void printMatrix(int const size, int const *matrix)
{
  if (matrix == NULL)
  {
    return;
  }
  for (int row = 0; row < size; row++)
  {
    for (int column = 0; column < size; column++)
    {
      int element = *(matrix + row * size + column);
      if (abs(element) < 10)
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

int main()
{
  int size;
  printf("Enter the size of the matrix: ");
  scanf("%d", &size);

  if (size <= 0)
  {
    printf("Error: Invalid Input\n");
    return 0;
  }

  // allocate memory for n^2 integers in one block
  int *matrix = malloc(size * size * sizeof(int));
  if (matrix == NULL)
  {
    printf("Memory Allocation failed!\n");
    return 1;
  }

  generateSpiralMatrix(size, matrix);
  printMatrix(size, matrix);

  free(matrix);
  return 0;
}