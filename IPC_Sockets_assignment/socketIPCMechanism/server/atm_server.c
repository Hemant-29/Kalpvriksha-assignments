#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>

#define SERVER_PORT 8080

pthread_mutex_t accountFileMutex;

void *handleClientRequest(void *arg)
{
  // Fix C: Cast the argument back to int (passed by value now)
  int clientSocket = *(int *)arg;
  free(arg); // Free the memory allocated in main

  int userChoice;
  int transactionAmount;
  int accountBalance;
  FILE *accountFilePointer;

  while (1)
  {
    int readStatus = read(clientSocket, &userChoice, sizeof(userChoice));
    if (readStatus <= 0 || userChoice == 4)
    {
      break; // Exit the loop to close socket
    }

    // Logic for Deposit/Withdraw
    if (userChoice == 1 || userChoice == 2)
    {
      read(clientSocket, &transactionAmount, sizeof(transactionAmount));
    }

    // --- CRITICAL SECTION START ---
    pthread_mutex_lock(&accountFileMutex);

    accountFilePointer = fopen("../accountDB.txt", "r");
    if (accountFilePointer == NULL)
    {
      // If file doesn't exist, assume balance 0 or handle error
      accountBalance = 0;
    }
    else
    {
      fscanf(accountFilePointer, "%d", &accountBalance);
      fclose(accountFilePointer);
    }

    if (userChoice == 1) // Withdraw
    {
      if (transactionAmount <= accountBalance)
      {
        accountBalance -= transactionAmount;
      }
    }
    else if (userChoice == 2) // Deposit
    {
      accountBalance += transactionAmount;
    }

    // Write new balance back
    accountFilePointer = fopen("../accountDB.txt", "w");
    fprintf(accountFilePointer, "%d", accountBalance);
    fclose(accountFilePointer);

    pthread_mutex_unlock(&accountFileMutex);
    // --- CRITICAL SECTION END ---

    // Send updated balance back to client
    write(clientSocket, &accountBalance, sizeof(accountBalance));
  }

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

  if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
  {
    perror("Bind failed");
    exit(1);
  }

  listen(serverSocket, 5);
  printf("Server listening on port %d...\n", SERVER_PORT);

  while (1)
  {
    struct sockaddr_in clientAddress;
    socklen_t clientLen = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLen);

    if (clientSocket < 0)
    {
      perror("Accept failed");
      continue;
    }

    int *pclient = malloc(sizeof(int));
    *pclient = clientSocket;

    pthread_t clientThread;
    pthread_create(&clientThread, NULL, handleClientRequest, pclient);
    pthread_detach(clientThread);
  }

  return 0;
}