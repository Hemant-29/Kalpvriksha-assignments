#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIMIT 1000
#define STACK_EMPTY -9999

// Generic stack implementation
typedef struct
{
  int top;
  int arr[MAX_LIMIT];
} Stack;

void initStack(Stack *s)
{
  s->top = -1;
}

int isEmptyStack(Stack *s)
{
  return s->top < 0;
}

void pushToStack(Stack *s, int val)
{
  if (s->top < MAX_LIMIT - 1)
  {
    s->arr[++s->top] = val;
  }
  else
  {
    printf("Stack overflow\n");
  }
}

int popStack(Stack *s)
{
  if (isEmptyStack(s))
  {
    printf("Stack underflow\n");
    return STACK_EMPTY;
  }
  return s->arr[s->top--];
}

int peekStack(Stack *s)
{
  if (isEmptyStack(s))
  {
    return STACK_EMPTY;
  }
  return s->arr[s->top];
}

// Function to return precedence of operators
int precedence(char op)
{
  if (op == '/' || op == '*')
    return 2;
  if (op == '+' || op == '-')
    return 1;
  return 0; // Should not happen for valid operators
}

// Function to convert infix expression to a postfix expression
int convertToPostfix(char expression[MAX_LIMIT])
{
  char postfix[MAX_LIMIT] = ""; // output string
  int expressionLength = strlen(expression);

  Stack st;
  initStack(&st);

  // process all the expression characters in a loop
  for (int i = 0; i < expressionLength; i++)
  {
    char currChar = expression[i];

    // Handling whitespaces inbetween the characters
    if (currChar == ' ')
    {
      continue;
    }

    // If the current character is a number, add it into postfix expression
    if (currChar >= '0' && currChar <= '9')
    {
      char tempStr[10] = "";
      while (i < expressionLength && expression[i] >= '0' && expression[i] <= '9')
      {
        char currCharStr[2] = {expression[i], '\0'};
        strcat(tempStr, currCharStr);
        i++;
      }
      i--;

      strcat(postfix, tempStr);
      strcat(postfix, " ");
      continue;
    }

    // If current element is an operator, then do precedence checks
    else if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/')
    {
      while (!isEmptyStack(&st) && precedence((char)peekStack(&st)) >= precedence(currChar))
      {
        char poppedElement = (char)popStack(&st);
        char tempStr[2] = {poppedElement, '\0'};
        strcat(postfix, tempStr);
        strcat(postfix, " ");
      }

      // if precedence of curr is more, or if theres no element in the stack
      pushToStack(&st, currChar);
    }

    // Handling the invalid input in case its not a number or an operator
    else
    {
      printf("Error: Invalid expression\n");
      return 0;
    }
  }

  // Once the loop is over, pop the remaining stack elements and add to the expression
  while (!isEmptyStack(&st))
  {
    char curr = (char)popStack(&st);
    char tempStr[2] = {curr, '\0'};
    strcat(postfix, tempStr);
    strcat(postfix, " ");
  }

  // copy the postfix output to the expression string
  strcpy(expression, postfix);
  return 1;
}

// Function to solve a postfix expression
int solvePostfix(char expression[MAX_LIMIT])
{
  int expLength = strlen(expression);
  Stack st;
  initStack(&st);

  for (int i = 0; i < expLength; i++)
  {
    char currChar = expression[i];
    if (currChar == ' ')
    {
      continue;
    }
    else if (currChar >= '0' && currChar <= '9')
    {
      int number = 0;
      while (i < expLength && expression[i] >= '0' && expression[i] <= '9')
      {
        number = number * 10 + (expression[i] - '0');
        i++;
      }
      i--; // Backtrack by one value
      pushToStack(&st, number);
    }
    else if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/')
    {
      // perform the operation on the 2 elements below it
      int result;
      int el1 = popStack(&st);
      int el2 = popStack(&st);
      if (el1 == STACK_EMPTY || el2 == STACK_EMPTY)
      {
        printf("Error: Invalid expression\n");
        return -1;
      }

      switch (currChar)
      {
      case '+':
        result = el2 + el1;
        break;
      case '-':
        result = el2 - el1;
        break;
      case '*':
        result = el2 * el1;
        break;
      case '/':
        if (el1 == 0)
        {
          printf("Divide by zero error\n");
          return -1;
        }
        result = el2 / el1;
        break;

      default:
        break;
      }

      // Push the result back into the stack
      pushToStack(&st, result);
    }
    else
    {
      printf("Error: Invalid expression\n");
      return -1;
    }
  }
  return popStack(&st);
}

int main()
{

  char expression[MAX_LIMIT] = "19 - 2 * 3 + 4 / 2";

  printf("Enter your expression: ");
  fgets(expression, sizeof(expression), stdin);
  expression[strcspn(expression, "\n")] = '\0';

  if (convertToPostfix(expression))
  {
    printf("Expression output is: %d\n", solvePostfix(expression));
  }

  return 0;
}
