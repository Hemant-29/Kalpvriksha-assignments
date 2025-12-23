#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

#define SERVER_PORT 8080

int main()
{
  int clientSocket;
  struct sockaddr_in serverAddress;

  int userChoice;
  int transactionAmount;
  int accountBalance;

  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0)
  {
    perror("Socket creation failed");
    exit(1);
  }

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(SERVER_PORT);
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

  while (1)
  {
    printf("\n1. Withdraw\n2. Deposit\n3. Display Balance\n4. Exit\nEnter choice: ");
    scanf("%d", &userChoice);

    // Handle Exit
    if (userChoice == 4)
    {
      write(clientSocket, &userChoice, sizeof(userChoice));
      break;
    }

    write(clientSocket, &userChoice, sizeof(userChoice));

    if (userChoice == 1 || userChoice == 2)
    {
      printf("Enter amount: ");
      scanf("%d", &transactionAmount);
      write(clientSocket, &transactionAmount, sizeof(transactionAmount));
    }

    read(clientSocket, &accountBalance, sizeof(accountBalance));
    printf("Current Balance: %d\n", accountBalance);
  }
  close(clientSocket);
  return 0;
}
