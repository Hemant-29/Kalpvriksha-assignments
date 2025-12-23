#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#define SERVER_PORT 8080

pthread_mutex_t accountFileMutex;

void *handleClientRequest(void *clientSocketPointer)
{
  int clientSocket = *(int *)clientSocketPointer;
  int userChoice;
  int transactionAmount;
  int accountBalance;

  FILE *accountFilePointer;

  pthread_mutex_lock(&accountFileMutex);
  accountFilePointer = fopen("../accountDB.txt", "r");
  if (accountFilePointer == NULL)
  {
    perror("Account file open failed");
    pthread_mutex_unlock(&accountFileMutex);
    close(clientSocket);
    return NULL;
  }
  fscanf(accountFilePointer, "%d", &accountBalance);
  fclose(accountFilePointer);
  pthread_mutex_unlock(&accountFileMutex);

  read(clientSocket, &userChoice, sizeof(userChoice));

  if (userChoice == 1)
  {
    read(clientSocket, &transactionAmount, sizeof(transactionAmount));
    if (transactionAmount <= accountBalance)
    {
      accountBalance -= transactionAmount;
    }
  }
  else if (userChoice == 2)
  {
    read(clientSocket, &transactionAmount, sizeof(transactionAmount));
    accountBalance += transactionAmount;
  }

  pthread_mutex_lock(&accountFileMutex);
  accountFilePointer = fopen("../accountDB.txt", "w");
  fprintf(accountFilePointer, "%d", accountBalance);
  fclose(accountFilePointer);
  pthread_mutex_unlock(&accountFileMutex);

  write(clientSocket, &accountBalance, sizeof(accountBalance));
  close(clientSocket);

  return NULL;
}

int main()
{
  int serverSocket;
  struct sockaddr_in serverAddress;

  pthread_mutex_init(&accountFileMutex, NULL);

  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0)
  {
    perror("Socket creation failed");
    exit(1);
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(SERVER_PORT);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  listen(serverSocket, 5);

  while (1)
  {
    int clientSocket = accept(serverSocket, NULL, NULL);
    pthread_t clientThread;
    pthread_create(&clientThread, NULL, handleClientRequest, &clientSocket);
    pthread_detach(clientThread);
  }

  return 0;
}
