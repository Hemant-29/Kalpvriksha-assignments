#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE_LIMIT 1000
#define GRADE_A 85
#define GRADE_B 70
#define GRADE_C 50
#define GRADE_D 35

typedef struct
{
  int RollNumber;
  char name[SIZE_LIMIT];
  int marksSubject1;
  int marksSubject2;
  int marksSubject3;
} Student;

int calculateTotal(const Student *student)
{
  if (!student)
  {
    return -1;
  }
  int total = student->marksSubject1 + student->marksSubject2 + student->marksSubject3;
  return total;
}

char fetchGrade(const float average)
{
  char grade = '\0';

  if (average >= GRADE_A)
  {
    grade = 'A';
  }
  else if (average >= GRADE_B)
  {
    grade = 'B';
  }
  else if (average >= GRADE_C)
  {
    grade = 'C';
  }
  else if (average >= GRADE_D)
  {
    grade = 'D';
  }
  else
  {
    grade = 'F';
  }

  return grade;
}

void printPerformance(const char grade)
{
  int starCount = 1;
  switch (grade)
  {
  case 'A':
    starCount = 5;
    break;
  case 'B':
    starCount = 4;
    break;
  case 'C':
    starCount = 3;
    break;
  case 'D':
    starCount = 2;
    break;
  default:
    break;
  }

  if (starCount > 1)
  {
    printf("Performance: ");
  }
  for (int star = 0; star < starCount; star++)
  {
    if (grade == 'F')
    {
      continue;
    }
    printf("*");
  }
  printf("\n");
}

void printRollNumber(const Student students[], int index, int studentCount)
{
  if (index >= studentCount)
  {
    return;
  }

  if (students[index].RollNumber)
  {
    printf("%d ", students[index].RollNumber);
  }
  index++;
  printRollNumber(students, index, studentCount);
}

void inputStudentDetails(Student students[], int studentCount)
{
  for (int studentIndex = 0; studentIndex < studentCount; studentIndex++)
  {
    printf("\nEnter: Roll No. Name Marks 1 Marks 2 Marks 3 (for student %d): \n", studentIndex + 1);
    int RollNumber;
    char name[SIZE_LIMIT];
    int marks1;
    int marks2;
    int marks3;
    scanf("%d %999s %d %d %d", &RollNumber, name, &marks1, &marks2, &marks3);

    if (RollNumber <= 0)
    {
      printf("Roll No. can't be negative, try again\n");
      studentIndex--;
      continue;
    }

    if (marks1 < 0 || marks1 > 100 || marks2 < 0 || marks2 > 100 || marks3 < 0 || marks3 > 100)
    {
      printf("Marks must be in range 0-100, try again\n");
      studentIndex--;
      continue;
    }

    if (strlen(name) == 0)
    {
      printf("Invalid Name! Cannot be empty.\n");
      studentIndex--;
      continue;
    }

    students[studentIndex].RollNumber = RollNumber;
    students[studentIndex].marksSubject1 = marks1;
    students[studentIndex].marksSubject2 = marks2;
    students[studentIndex].marksSubject3 = marks3;
    strcpy(students[studentIndex].name, name);
  }
}

void printResults(Student students[], int studentCount)
{
  for (int student = 0; student < studentCount; student++)
  {
    int total = calculateTotal(&students[student]);
    float average = total / 3.0;
    char grade = fetchGrade(average);

    printf("\nRoll no: %d\n", students[student].RollNumber);
    printf("Name: %s\n", students[student].name);
    printf("Total marks: %d\n", total);
    printf("Average: %.2f\n", average);
    printf("Grade: %c\n", grade);

    printPerformance(grade);
  }
}

int main()
{
  int studentCount = 0;

  printf("No. of students: ");
  scanf("%d", &studentCount);

  Student students[studentCount];

  inputStudentDetails(students, studentCount);

  printResults(students, studentCount);

  printf("List of Roll Numbers (via recursion): ");
  printRollNumber(students, 0, studentCount);

  return 0;
}

