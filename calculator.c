#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STACK_CAPACITY 1000

typedef struct
{
  int top;
  int items[STACK_CAPACITY];
} Stack;

void initStack(Stack *s)
{
  s->top = -1;
}

int isEmpty(const Stack *s)
{

  return s->top < 0;
}

void push(Stack *s, int value)
{
  if (s->top < STACK_CAPACITY - 1)
    s->items[++s->top] = value;
}

int pop(Stack *s, int *value)
{
  if (isEmpty(s) || value == NULL) // Check for NULL pointer
  {
    return 0; // Failure
  }
  *value = s->items[s->top--];
  return 1; // Success
}

int peek(const Stack *s, int *value)
{
  if (isEmpty(s) || value == NULL)
  {
    return 0; // Failure
  }
  *value = s->items[s->top];
  return 1; // Success
}

int getOperatorPrecedence(char operatorChar)
{
  if (operatorChar == '*' || operatorChar == '/')
    return 2;
  if (operatorChar == '+' || operatorChar == '-')
    return 1;
  return 0;
}

int applyOperator(int operand1, int operand2, char operator, int * errorFlag)
{
  switch (operator)
  {
  case '+':
    return operand1 + operand2;
  case '-':
    return operand1 - operand2;
  case '*':
    return operand1 * operand2;
  case '/':
    if (operand2 == 0)
    {
      *errorFlag = 1;
      return 0;
    }
    return operand1 / operand2;
  default:
    *errorFlag = 1;
    return 0;
  }
}

int evaluateExpression(const char *expression, int *errorFlag)
{
  Stack operandStack;
  Stack operatorStack;
  initStack(&operandStack);
  initStack(&operatorStack);

  int i = 0;
  while (expression[i] != '\0')
  {
    if (isspace((unsigned char)expression[i]))
    {
      i++;
      continue;
    }

    if (isdigit((unsigned char)expression[i]))
    {
      int currentNumber = 0;
      while (isdigit((unsigned char)expression[i]))
      {
        currentNumber = currentNumber * 10 + (expression[i] - '0');
        i++;
      }
      push(&operandStack, currentNumber);
    }
    else if (strchr("+-*/", expression[i]))
    {
      char currentOperator = expression[i];
      while (!isEmpty(&operatorStack))
      {
        int peekedOperator;
        if (!peek(&operatorStack, &peekedOperator))
        {
          *errorFlag = 1;
          return 0;
        }
        char topOp = (char)peekedOperator;
        if (getOperatorPrecedence(topOp) >= getOperatorPrecedence(currentOperator))
        {
          int operand2;
          if (!pop(&operandStack, &operand2))
          {
            *errorFlag = 1;
            return 0;
          }
          // int operand1 = pop(&operandStack);
          int operand1;
          if (!pop(&operandStack, &operand1))
          {
            *errorFlag = 1;
            return 0;
          }

          int poppedOperator;
          if (!pop(&operatorStack, &poppedOperator))
          {
            *errorFlag = 1;
            return 0; // Error: unexpected empty stack
          }

          char operatorChar = (char)poppedOperator;

          int operationResult = applyOperator(operand1, operand2, operatorChar, errorFlag);
          if (*errorFlag)
            return 0;
          push(&operandStack, operationResult);
        }
        else
          break;
      }
      push(&operatorStack, (int)currentOperator); // store char as int
      i++;
    }
    else
    {
      *errorFlag = 1; // invalid character
      return 0;
    }
  }

  while (!isEmpty(&operatorStack))
  {
    int operand2;
    if (!pop(&operandStack, &operand2))
    {
      *errorFlag = 1;
      return 0;
    }

    int operand1;
    if (!pop(&operandStack, &operand1))
    {
      *errorFlag = 1;
      return 0;
    }

    int poppedOperator;
    if (!pop(&operatorStack, &poppedOperator))
    {
      *errorFlag = 1;
      return 0;
    }

    char operatorChar = (char)poppedOperator;
    int operationResult = applyOperator(operand1, operand2, operatorChar, errorFlag);
    if (*errorFlag)
      return 0;
    push(&operandStack, operationResult);
  }

  int finalResult;
  if (!pop(&operandStack, &finalResult))
  {
    *errorFlag = 1;
    return 0;
  }
  return finalResult;
}

int main(void)
{
  char expression[STACK_CAPACITY];
  printf("Enter your expression: ");
  if (!fgets(expression, sizeof(expression), stdin))
  {
    printf("Error reading input.\n");
    return 1;
  }
  expression[strcspn(expression, "\n")] = '\0';

  int errorFlag = 0;
  int result = evaluateExpression(expression, &errorFlag);

  if (errorFlag)
    printf("Error: Invalid expression or division by zero.\n");
  else
    printf("Result: %d\n", result);

  return 0;
}
