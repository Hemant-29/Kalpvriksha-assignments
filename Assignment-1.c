#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define limit 100

int checkValid(char ch)
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

int isNumber(char ch)
{
  if (ch >= '0' && ch <= '9' || ch == ' ')
  {
    return 1;
  }
  return 0;
}

int performOperation(char expression[limit], int length, char op1, char op2)
{

  int st_index = 0;
  int end_index = 0;
  char operator= '\0';
  int op_index = 0;

  // ___Traversing the whole expression string
  for (int i = 0; i < length; i++)
  {
    // When we come across a * or / operator
    if (expression[i] == op1 || expression[i] == op2)
    {
      operator= expression[i];
      op_index = i;
      // Then we find the 'starting index' and the 'ending index'

      // finding starting index (go left until non-number)
      int left_end = i - 1;
      while (left_end >= 0 && isNumber(expression[left_end]))
        left_end--;
      st_index = left_end + 1;

      // finding ending index (go right until non-number)
      int right_end = i + 1;
      while (right_end < length && isNumber(expression[right_end]))
        right_end++;
      end_index = right_end - 1;

      break;
    }
  }

  // ___Calculating the operation and replacing the value

  // Finding the num1 and num2
  char num1[limit] = "";
  int n1idx = 0;
  for (int k = st_index; k < op_index; k++)
  {
    if (expression[k] != ' ')
      num1[n1idx++] = expression[k]; // Skipping any whitespaces
  }
  num1[n1idx] = '\0';

  char num2[limit] = "";
  int n2idx = 0;
  for (int k = op_index + 1; k <= end_index; k++)
  {
    if (expression[k] != ' ')
    {
      num2[n2idx++] = expression[k];
    }
  }
  num2[n2idx] = '\0';

  // printf("num1 is: %s\n", num1);
  // printf("num2 is: %s\n", num2);

  // performing the operation
  int op_result = 0;
  if (operator== '+')
    op_result = atoi(num1) + atoi(num2);
  if (operator== '-')
    op_result = atoi(num1) - atoi(num2);
  if (operator== '*')
    op_result = atoi(num1) * atoi(num2);
  if (operator== '/')
  {
    if (atoi(num2) == 0)
    {
      printf("division by zero error\n");
      return -1;
    }
    op_result = atoi(num1) / atoi(num2);
  }

  // Convert operation result into string
  char result_str[limit] = "";
  sprintf(result_str, "%d", op_result);
  // printf("result is: %s\n", result_str);

  // place the operation result back into the expression string
  int result_len = strlen(result_str);
  int tmp = st_index;
  int i = 0;
  while (tmp <= end_index)
  {
    if (i < result_len)
    {
      expression[tmp] = result_str[i];
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

int calculate(char expression[limit], int length)
{
  // Find the number of divMul and addSub operations
  int no_addSub = 0;
  int no_mulDiv = 0;

  for (int i = 0; i < length; i++)
  {
    if (!checkValid(expression[i]))
    {
      printf("Invalid input expression");
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
  char expression[limit];

  // Input expression
  printf("Enter the expression: ");
  fgets(expression, sizeof(expression), stdin);
  expression[strcspn(expression, "\n")] = '\0';

  // find length
  int length = strlen(expression);

  // Calculate
  printf("result: %d", calculate(expression, length));
  return 0;
}