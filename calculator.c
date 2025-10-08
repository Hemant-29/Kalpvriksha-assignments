#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LIMIT 1000
#define STACK_EMPTY -9999

typedef struct
{
  int top;
  int items[MAX_LIMIT];
} IntegerStack;

typedef struct
{
  int top;
  char items[MAX_LIMIT];
} CharacterStack;

void initializeIntegerStack(IntegerStack *stack)
{
  stack->top = -1;
}

int isIntegerStackEmpty(const IntegerStack *stack)
{
  return stack->top < 0;
}

void pushToIntegerStack(IntegerStack *stack, int value)
{
  if (stack->top < MAX_LIMIT - 1)
  {
    stack->items[++stack->top] = value;
  }
}

int popFromIntegerStack(IntegerStack *stack)
{
  if (isIntegerStackEmpty(stack))
  {
    return STACK_EMPTY;
  }
  return stack->items[stack->top--];
}

void initializeCharacterStack(CharacterStack *stack)
{
  stack->top = -1;
}

int isCharacterStackEmpty(const CharacterStack *stack)
{
  return stack->top < 0;
}

void pushToCharacterStack(CharacterStack *stack, char value)
{
  if (stack->top < MAX_LIMIT - 1)
  {
    stack->items[++stack->top] = value;
  }
}

char popFromCharacterStack(CharacterStack *stack)
{
  if (isCharacterStackEmpty(stack))
  {
    return '\0';
  }
  return stack->items[stack->top--];
}

char peekAtCharacterStack(const CharacterStack *stack)
{
  if (isCharacterStackEmpty(stack))
  {
    return '\0';
  }
  return stack->items[stack->top];
}

int getOperatorPrecedence(char operator)
{
  if (operator== '*' || operator== '/')
  {
    return 2;
  }
  if (operator== '+' || operator== '-')
  {
    return 1;
  }
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
  IntegerStack valueStack;
  CharacterStack operatorStack;
  initializeIntegerStack(&valueStack);
  initializeCharacterStack(&operatorStack);

  int i = 0;
  while (expression[i] != '\0')
  {
    if (isspace(expression[i]))
    {
      i++;
      continue;
    }

    if (isdigit(expression[i]))
    {
      int currentNumber = 0;
      while (isdigit(expression[i]))
      {
        currentNumber = currentNumber * 10 + (expression[i] - '0');
        i++;
      }
      pushToIntegerStack(&valueStack, currentNumber);
    }
    else if (strchr("+-*/", expression[i]))
    {
      while (!isCharacterStackEmpty(&operatorStack) &&
             getOperatorPrecedence(peekAtCharacterStack(&operatorStack)) >= getOperatorPrecedence(expression[i]))
      {
        int operand2 = popFromIntegerStack(&valueStack);
        int operand1 = popFromIntegerStack(&valueStack);
        char operator= popFromCharacterStack(&operatorStack);

        if (operand1 == STACK_EMPTY || operand2 == STACK_EMPTY)
        {
          *errorFlag = 1;
          return 0;
        }

        pushToIntegerStack(&valueStack, applyOperator(operand1, operand2, operator, errorFlag));
        if (*errorFlag)
        {
          return 0;
        }
      }
      pushToCharacterStack(&operatorStack, expression[i]);
      i++;
    }
    else
    {
      *errorFlag = 1; // Invalid character
      return 0;
    }
  }

  while (!isCharacterStackEmpty(&operatorStack))
  {
    int operand2 = popFromIntegerStack(&valueStack);
    int operand1 = popFromIntegerStack(&valueStack);
    char operator= popFromCharacterStack(&operatorStack);

    if (operand1 == STACK_EMPTY || operand2 == STACK_EMPTY)
    {
      *errorFlag = 1;
      return 0;
    }

    pushToIntegerStack(&valueStack, applyOperator(operand1, operand2, operator, errorFlag));
    if (*errorFlag)
    {
      return 0;
    }
  }

  return popFromIntegerStack(&valueStack);
}

int main()
{
  char expression[MAX_LIMIT];
  printf("Enter your expression: ");
  fgets(expression, sizeof(expression), stdin);
  expression[strcspn(expression, "\n")] = '\0';

  int errorFlag = 0;
  int result = evaluateExpression(expression, &errorFlag);

  if (errorFlag)
  {
    printf("Error: Invalid expression or division by zero.\n");
  }
  else
  {
    printf("Result: %d\n", result);
  }

  return 0;
}