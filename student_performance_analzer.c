#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE_LIMIT 1000

typedef struct
{
  int rollNo;
  char name[SIZE_LIMIT];
  int marksSubject1;
  int marksSubject2;
  int marksSubject3;
} Student;

int calculateTotal(Student *s)
{
  int total = s->marksSubject1 + s->marksSubject2 + s->marksSubject3;
  return total;
}

char fetchGrade(float average)
{
  char grade = '\0';

  if (average >= 85)
  {
    grade = 'A';
  }
  else if (average >= 70)
  {
    grade = 'B';
  }
  else if (average >= 50)
  {
    grade = 'C';
  }
  else if (average >= 35)
  {
    grade = 'D';
  }
  else
  {
    grade = 'F';
  }

  return grade;
}

void printPerformance(char grade)
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

void recursivePrintRollNo(Student studentsArray[], int index, int studentCount)
{
  if (index >= studentCount)
  {
    return;
  }

  if (studentsArray[index].rollNo)
  {
    printf("%d ", studentsArray[index].rollNo);
  }
  index++;
  recursivePrintRollNo(studentsArray, index, studentCount);
}

int main()
{
  int studentCount = 0;

  printf("No. of students: ");
  scanf("%d", &studentCount);

  Student studentsArray[studentCount];

  for (int currStudent = 0; currStudent < studentCount; currStudent++)
  {
    printf("\nEnter: Roll No. Name Marks 1 Marks 2 Marks 3 (for student %d): \n", currStudent + 1);
    int rollNo;
    char name[SIZE_LIMIT];
    int marks1;
    int marks2;
    int marks3;
    scanf("%d %s %d %d %d", &rollNo, name, &marks1, &marks2, &marks3);

    Student s;
    s.rollNo = rollNo;
    s.marksSubject1 = marks1;
    s.marksSubject2 = marks2;
    s.marksSubject3 = marks3;
    strcpy(s.name, name);

    // push current student to students array
    studentsArray[currStudent] = s;
  }

  for (int student = 0; student < studentCount; student++)
  {
    int total = calculateTotal(&studentsArray[student]);
    float average = total / 3.0;
    char grade = fetchGrade(average);

    printf("\nRoll no: %d\n", studentsArray[student].rollNo);
    printf("Name: %s\n", studentsArray[student].name);
    printf("Total marks: %d\n", total);
    printf("Average: %f\n", average);
    printf("Grade: %c\n", grade);

    printPerformance(grade);
  }

  printf("List of Roll Numbers (via recursion): ");
  recursivePrintRollNo(studentsArray, 0, studentCount);

  return 0;
}
