#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// File operations
void writeToFile(char filename[1000], char str[1000])
{
  FILE *fptr;
  fptr = fopen(filename, "w");

  if (fptr != NULL)
  {
    fputs(str, fptr);
    fclose(fptr);
  }
  else
  {
    printf("Error: Can't open file");
  }
}

void appendToFile(char filename[1000], char str[1000])
{
  FILE *fptr;
  fptr = fopen(filename, "a");

  if (fptr != NULL)
  {
    fputs(str, fptr);
    fclose(fptr);
  }
  else
  {
    printf("Error: Can't open file!");
  }
}

void readFromFile(char filename[1000])
{
  FILE *fptr;
  fptr = fopen(filename, "r");

  if (fptr != NULL)
  {
    char content[1000];
    while (fgets(content, sizeof(content), fptr))
    {
      printf("%s", content);
    }
    fclose(fptr);
  }
  else
  {
    printf("Error: Can't open file");
  }
}

// User struct
struct User
{
  int id;
  char name[1000];
  int age;
};

int loadUsers(const char *filename, struct User users[])
{
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL)
  {
    printf("Error: Can't open file for reading!\n");
    return 0;
  }

  int count = 0;
  // assign id, name, age to the users[] array
  while (fscanf(fptr, "%d %s %d",
                &users[count].id,
                users[count].name,
                &users[count].age) == 3)
  {
    count++;
  }

  fclose(fptr);
  return count; // number of users actually loaded
}

void getUsers()
{
  printf("\nid Username age\n");
  readFromFile("users.txt");
}

void addUser(int id, char name[1000], int age)
{
  // Load existing users
  struct User users[1000];
  int count = loadUsers("users.txt", users);

  // Check for duplicate ID
  for (int i = 0; i < count; i++)
  {
    if (users[i].id == id)
    {
      printf("Error: User with ID %d already exists!\n", id);
      return; // don’t add the user
    }
  }

  // If no duplicate found, create new user
  struct User username;
  username.id = id;
  strcpy(username.name, name);
  username.age = age;

  // Append to the file
  char combinedStr[1000];
  sprintf(combinedStr, "%d %s %d\n", id, name, age);
  appendToFile("users.txt", combinedStr);

  printf("User with ID %d added successfully!\n", id);
}

void updateUser(int id, char name[1000], int age)
{
  struct User users[1000];
  int count = loadUsers("users.txt", users);

  int found = 0;
  for (int i = 0; i < count; i++)
  {
    if (users[i].id == id)
    {
      strcpy(users[i].name, name);
      users[i].age = age;
      found = 1;
      break;
    }
  }

  if (!found)
  {
    printf("User with ID %d not found! Cannot update.\n", id);
    return;
  }

  // Re-write users back into the file
  writeToFile("users.txt", "");
  for (int i = 0; i < count; i++)
  {
    char combinedStr[1000];
    sprintf(combinedStr, "%d %s %d\n", users[i].id, users[i].name, users[i].age);
    appendToFile("users.txt", combinedStr);
  }

  printf("User with ID %d updated successfully!\n", id);
}

void deleteUser(int id)
{
  struct User users[1000];
  int count = loadUsers("users.txt", users);

  int found = 0;
  for (int i = 0; i < count; i++)
  {
    if (users[i].id == id)
    {
      found = 1;

      // Shift users down to remove this one
      for (int j = i; j < count - 1; j++)
      {
        users[j] = users[j + 1];
      }
      count--;
      printf("User with ID %d deleted successfully!\n", id);
      break;
    }
  }

  if (!found)
  {
    printf("User with ID %d not found! Cannot delete.\n", id);
    return;
  }

  // Rewrite updated list
  writeToFile("users.txt", "");
  for (int i = 0; i < count; i++)
  {
    char combinedStr[1000];
    sprintf(combinedStr, "%d %s %d\n", users[i].id, users[i].name, users[i].age);
    appendToFile("users.txt", combinedStr);
  }
}

int main()
{

  int choice, id, age;
  char name[1000];
  while (1)
  {
    printf("\n1. Add\n2. Read\n3. Update\n4. Delete\n5. Exit\nChoice: ");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
      printf("Enter ID Name Age: ");
      scanf("%d %s %d", &id, name, &age);
      addUser(id, name, age);
      break;
    case 2:
      getUsers();
      break;
    case 3:
      printf("Enter ID NewName NewAge: ");
      scanf("%d %s %d", &id, name, &age);
      updateUser(id, name, age);
      break;
    case 4:
      printf("Enter ID to delete: ");
      scanf("%d", &id);
      deleteUser(id);
      break;
    case 5:
      exit(0);
    default:
      printf("Invalid choice!\n");
    }
  }

  return 0;
}