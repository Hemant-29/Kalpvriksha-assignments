#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIMIT 100

int isValidCharacter(char ch)
{
  char valids[15] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '-', '*', '/', ' '};
  for (int i = 0; i < 15; i++)
  {
    if (ch == valids[i])
      return 1; // ch present in valids array
  }
  return 0;
}

int isOperator(char ch)
{
  if (ch == '*' || ch == '/' || ch == '+' || ch == '-')
  {
    return 1;
  }
  return 0;
}

int isOperandCharacter(char ch)
{
  return (ch >= '0' && ch <= '9') || (ch == ' ');
}

int performOperation(char expression[LIMIT], int length, char op1, char op2)
{

  int startIndex = 0;
  int endIndex = 0;
  char operator= '\0';
  int operatorIndex = 0;

  for (int i = 0; i < length; i++)
  {
    if (expression[i] == op1 || expression[i] == op2)
    {
      operator= expression[i];
      operatorIndex = i;

      // finding starting index (go left until non-number)
      int leftEnd = i - 1;
      while (leftEnd >= 0 && isOperandCharacter(expression[leftEnd]))
      {
        leftEnd--;
      }
      startIndex = leftEnd + 1;

      // finding ending index (go right until non-number)
      int rightEnd = i + 1;
      while (rightEnd < length && isOperandCharacter(expression[rightEnd]))
      {
        rightEnd++;
      }
      endIndex = rightEnd - 1;

      break;
    }
  }

  // Finding the num1 and num2
  char num1[LIMIT] = "";
  int n1Index = 0;
  for (int k = startIndex; k < operatorIndex; k++)
  {
    if (expression[k] != ' ')
    {
      num1[n1Index] = expression[k]; // Skipping any whitespaces
      n1Index++;
    }
  }
  num1[n1Index] = '\0';

  char num2[LIMIT] = "";
  int n2Index = 0;
  for (int k = operatorIndex + 1; k <= endIndex; k++)
  {
    if (expression[k] != ' ')
    {
      num2[n2Index++] = expression[k];
    }
  }
  num2[n2Index] = '\0';

  // performing the operation
  int operationResult = 0;
  switch (operator)
  {
  case '+':
    operationResult = atoi(num1) + atoi(num2);
    break;
  case '-':
    operationResult = atoi(num1) - atoi(num2);
    break;
  case '*':
    operationResult = atoi(num1) * atoi(num2);
    break;
  case '/':
    if (atoi(num2) == 0)
    {
      printf("Division by zero error\n");
      return -1;
    }
    operationResult = atoi(num1) / atoi(num2);
    break;
  default:
    printf("Invalid operator\n");
    return -1;
  }

  // Convert operation result into string
  char resultString[LIMIT] = "";
  sprintf(resultString, "%d", operationResult);

  // place the operation result back into the expression string
  int resultLength = strlen(resultString);
  int tmp = startIndex;
  int i = 0;
  while (tmp <= endIndex)
  {
    if (i < resultLength)
    {
      expression[tmp] = resultString[i];
      i++;
    }
    else
    {
      expression[tmp] = ' ';
    }
    tmp++;
  }

  return atoi(expression);
}

int evaluateExpressionDMAS(char expression[LIMIT], int length)
{
  // Find the number of divMul and addSub operations
  int no_addSub = 0;
  int no_mulDiv = 0;

  for (int i = 0; i < length; i++)
  {
    if (!isValidCharacter(expression[i]))
    {
      printf("Invalid input expression\n");
      return -1;
    }

    if (expression[i] == '*' || expression[i] == '/')
      no_mulDiv++;

    if (expression[i] == '+' || expression[i] == '-')
      no_addSub++;
  }

  // Perform the mul-div operations first and then add-sub operations
  int result = -1;
  for (int i = 0; i < no_mulDiv; i++)
  {
    result = performOperation(expression, length, '*', '/');
    if (result == -1)
      return -1;
  }
  for (int i = 0; i < no_addSub; i++)
  {
    result = performOperation(expression, length, '+', '-');
  }
  return result;
}

int main()
{
  // Define expression
  char expression[LIMIT];

  // Input expression
  printf("Enter the expression: ");
  fgets(expression, sizeof(expression), stdin);
  expression[strcspn(expression, "\n")] = '\0';

  // find length
  int length = strlen(expression);

  // execute Operations in the DMAS order
  printf("result: %d", evaluateExpressionDMAS(expression, length));
  return 0;
}