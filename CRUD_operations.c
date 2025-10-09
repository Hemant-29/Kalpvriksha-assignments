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

int loadUsersFromFile(const char *fileName, struct User userList[])
{
  FILE *filePointer = fopen(fileName, "rb");
  if (!filePointer)
  {
    return 0;
  }

  int userCount = 0;
  while (userCount < SIZE_LIMIT && fread(&userList[userCount], sizeof(struct User), 1, filePointer) == 1)
  {
    userCount++;
  }

  fclose(filePointer);
  return userCount;
}

int writeToFile(const char *fileName, struct User *data, int count, int append)
{

  FILE *filePointer;

  if (append)
  {
    filePointer = fopen(fileName, "ab");
  }
  else
  {
    filePointer = fopen(fileName, "wb");
  }

  if (!filePointer)
  {
    printf("Error: Could not open file for %s!\n", append ? "appending" : "writing");
    return 0;
  }

  if (count > 0)
  {
    int written = fwrite(data, sizeof(struct User), count, filePointer);
    if (written != count)
    {
      printf("Warning: wrote %d of %d records.\n", written, count);
      fclose(filePointer);
      return 0;
    }
  }

  fclose(filePointer);
  return 1;
}

void displayAllUsers()
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.dat", userList);

  if (userCount == 0)
  {
    printf("No users found!\n");
    return;
  }

  for (int i = 0; i < userCount; i++)
  {
    printf("ID: %d, Name: %s, Age: %d\n", userList[i].id, userList[i].name, userList[i].age);
  }
}

void addUser(int id, const char *name, int age)
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.dat", userList);

  for (int i = 0; i < userCount; i++)
  {
    if (userList[i].id == id)
    {
      printf("Error: User with ID %d already exists!\n", id);
      return;
    }
  }

  struct User newUser;
  newUser.id = id;
  strcpy(newUser.name, name);
  newUser.name[SIZE_LIMIT - 1] = '\0';
  newUser.age = age;

  if (!writeToFile("users.dat", &newUser, 1, 1)) // append
  {
    printf("Error: Could not open file for appending!\n");
    return;
  }

  printf("User with ID %d added successfully!\n", id);
}

void updateUser(int id, const char *name, int age)
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.dat", userList);
  int wasUserFound = 0;

  for (int i = 0; i < userCount; i++)
  {
    if (userList[i].id == id)
    {
      strcpy(userList[i].name, name);
      userList[i].name[SIZE_LIMIT - 1] = '\0';
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

  // Rewrite binary file with updated data
  if (!writeToFile("users.dat", userList, userCount, 0)) // overwrite
  {
    printf("Error: Could not open file for writing!\n");
    return;
  }

  printf("User with ID %d updated successfully!\n", id);
}

void deleteUser(int id)
{
  struct User userList[SIZE_LIMIT];
  int userCount = loadUsersFromFile("users.dat", userList);
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

  // Rewrite binary file with remaining users
  if (!writeToFile("users.dat", userList, userCount, 0)) // overwrite
  {
    printf("Error: Could not open file for writing!\n");
    return;
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
