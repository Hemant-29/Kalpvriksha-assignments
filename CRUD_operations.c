#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_LIMIT 1000

struct User
{
  int id;
  char name[SIZE_LIMIT];
  int age;
};

void writeToFile(const char *fileName, const char *content, const char *mode)
{
  FILE *filePointer = fopen(fileName, mode);
  if (filePointer != NULL)
  {
    fputs(content, filePointer);
    fclose(filePointer);
  }
  else
  {
    printf("Error: Could not open file!\n");
  }
}

void readFromFile(const char *fileName)
{
  FILE *filePointer = fopen(fileName, "r");
  if (filePointer != NULL)
  {
    char fileContent[SIZE_LIMIT];
    while (fgets(fileContent, sizeof(fileContent), filePointer))
    {
      printf("%s", fileContent);
    }
    fclose(filePointer);
  }
  else
  {
    printf("Error: Could not open file!\n");
  }
}

int loadUsersFromFile(const char *fileName, struct User userList[])
{
  FILE *filePointer = fopen(fileName, "r");
  if (!filePointer)
  {
    return 0;
  }

  int userCount = 0;
  // Format string reads up to 999 chars until a ',' to prevent buffer overflow.
  while (fscanf(filePointer, " {id: %d, name: %999[^,], age: %d}",
                &userList[userCount].id, userList[userCount].name, &userList[userCount].age) == 3)
  {
    userCount++;
    if (userCount >= SIZE_LIMIT)
    {
      break;
    }
  }

  fclose(filePointer);
  return userCount;
}

void displayAllUsers()
{
  readFromFile("users.txt");
}

void addUser(int id, const char *name, int age)
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.txt", userList);

  for (int i = 0; i < userCount; i++)
  {
    if (userList[i].id == id)
    {
      printf("Error: User with ID %d already exists!\n", id);
      return;
    }
  }

  char formattedUserData[SIZE_LIMIT];
  sprintf(formattedUserData, "{id: %d, name: %s, age: %d}\n", id, name, age);
  writeToFile("users.txt", formattedUserData, "a");

  printf("User with ID %d added successfully!\n", id);
}

void updateUser(int id, const char *name, int age)
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.txt", userList);
  int wasUserFound = 0;

  for (int i = 0; i < userCount; i++)
  {
    if (userList[i].id == id)
    {
      strcpy(userList[i].name, name);
      userList[i].age = age;
      wasUserFound = 1;
      break;
    }
  }

  if (!wasUserFound)
  {
    printf("User with ID %d not found! Cannot update.\n", id);
    return;
  }

  writeToFile("users.txt", "", "w"); // Clear the file before rewriting
  for (int i = 0; i < userCount; i++)
  {
    char formattedUserData[SIZE_LIMIT];
    sprintf(formattedUserData, "{id: %d, name: %s, age: %d}\n", userList[i].id, userList[i].name, userList[i].age);
    writeToFile("users.txt", formattedUserData, "a");
  }

  printf("User with ID %d updated successfully!\n", id);
}

void deleteUser(int id)
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.txt", userList);
  int wasUserFound = 0;

  for (int i = 0; i < userCount; i++)
  {
    if (userList[i].id == id)
    {
      wasUserFound = 1;
      // Shift elements down to overwrite and remove the user.
      for (int j = i; j < userCount - 1; j++)
      {
        userList[j] = userList[j + 1];
      }
      userCount--;
      printf("User with ID %d deleted successfully!\n", id);
      break;
    }
  }

  if (!wasUserFound)
  {
    printf("User with ID %d not found! Cannot delete.\n", id);
    return;
  }

  writeToFile("users.txt", "", "w"); // Clear the file before rewriting
  for (int i = 0; i < userCount; i++)
  {
    char formattedUserData[SIZE_LIMIT];
    sprintf(formattedUserData, "{id: %d, name: %s, age: %d}\n", userList[i].id, userList[i].name, userList[i].age);
    writeToFile("users.txt", formattedUserData, "a");
  }
}

int main()
{
  int choice, id, age;
  char name[SIZE_LIMIT];

  while (1)
  {
    printf("\n1. Add User\n2. Display Users\n3. Update User\n4. Delete User\n5. Exit\nChoice: ");
    scanf("%d", &choice);

    // Consume the leftover newline character before using fgets.
    int bufferChar;
    while ((bufferChar = getchar()) != '\n' && bufferChar != EOF)
      ;

    switch (choice)
    {
    case 1:
      printf("Enter ID: ");
      scanf("%d", &id);
      while ((bufferChar = getchar()) != '\n' && bufferChar != EOF)
        ;

      printf("Enter Name: ");
      fgets(name, sizeof(name), stdin);
      name[strcspn(name, "\n")] = '\0'; // Trim trailing newline.

      printf("Enter Age: ");
      scanf("%d", &age);
      addUser(id, name, age);
      break;

    case 2:
      displayAllUsers();
      break;

    case 3:
      printf("Enter ID of user to update: ");
      scanf("%d", &id);
      while ((bufferChar = getchar()) != '\n' && bufferChar != EOF)
        ;

      printf("Enter new Name: ");
      fgets(name, sizeof(name), stdin);
      name[strcspn(name, "\n")] = '\0';

      printf("Enter new Age: ");
      scanf("%d", &age);
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