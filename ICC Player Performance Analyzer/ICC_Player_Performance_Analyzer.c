#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ICC_header.h"

void addPlayer(TeamNode **teamsArray, int teamsCount)
{
  int TeamID;
  int PlayerId;
  char PlayerName[MAX_NAME];
  int PlayerRole;
  int TotalRuns;
  float BattingAverage;
  float StrikeRate;
  int Wickets;
  float EconomyRate;

  printf("Enter team ID: ");
  scanf("%d", &TeamID);
  TeamNode *team = searchTeamByID(teamsArray, teamsCount, TeamID);
  if (team == NULL)
  {
    printf("Team with id %d not found\n", TeamID);
    return;
  }
  if (team->TotalPlayers >= MAX_PLAYERS_PER_TEAM)
  {
    printf("Team full, can't add more players!\n");
    return;
  }

  printf("Enter player details:\n");

  printf("Enter player ID: ");
  scanf("%d", &PlayerId);
  if (findPlayerByID(teamsArray, teamsCount, PlayerId) == 1)
  {
    printf("This player ID already exists\n");
    return;
  }

  printf("Enter player name: ");
  getchar();
  if (scanf("%[^\n]", PlayerName) != 1)
  {
    printf("Invalid name\n");
    return;
  }

  printf("Enter player role(1-Batsman,2-Bowler,3-All Rounder): ");
  scanf("%d", &PlayerRole);
  if (PlayerRole < 1 || PlayerRole > 3)
  {
    printf("Invalid Role\n");
    return;
  }
  char roleName[MAX_ROLE_NAME];
  if (PlayerRole == 1)
  {
    strncpy(roleName, "Batsman", MAX_ROLE_NAME);
  }
  else if (PlayerRole == 2)
  {
    strncpy(roleName, "Bowler", MAX_ROLE_NAME);
  }
  else if (PlayerRole == 3)
  {
    strncpy(roleName, "All-rounder", MAX_ROLE_NAME);
  }

  printf("Enter total runs: ");
  scanf("%d", &TotalRuns);
  if (TotalRuns < 0)
  {
    printf("Invalid runs\n");
    return;
  }

  printf("Enter batting average: ");
  scanf("%f", &BattingAverage);
  if (BattingAverage < 0)
  {
    printf("Invalid batting average\n");
    return;
  }

  printf("Enter strike rate: ");
  scanf("%f", &StrikeRate);
  if (StrikeRate < 0)
  {
    printf("Invalid strike rate\n");
    return;
  }

  printf("Enter total wikets: ");
  scanf("%d", &Wickets);
  if (Wickets < 0)
  {
    printf("Wickets less than zero\n");
    return;
  }

  printf("Enter economy rate: ");
  scanf("%f", &EconomyRate);
  if (EconomyRate < 0)
  {
    printf("Invalid economy rate\n");
    return;
  }

  PlayerNode *newPlayer = malloc(sizeof *newPlayer);
  newPlayer->next = NULL;
  newPlayer->PlayerId = PlayerId;
  strncpy(newPlayer->Name, PlayerName, MAX_NAME);
  strncpy(newPlayer->TeamName, team->Name, MAX_NAME);
  strncpy(newPlayer->Role, roleName, MAX_ROLE_NAME);
  newPlayer->TotalRuns = TotalRuns;
  newPlayer->BattingAverage = BattingAverage;
  newPlayer->StrikeRate = StrikeRate;
  newPlayer->Wickets = Wickets;
  newPlayer->EconomyRate = EconomyRate;
  newPlayer->PerformanceIndex = calculatePerformanceIndex(newPlayer->Role, newPlayer->BattingAverage, newPlayer->StrikeRate, newPlayer->Wickets, newPlayer->EconomyRate);

  PlayerNode *head = NULL;
  if (PlayerRole == 1)
  {
    head = team->batsmanHead;
    if (head == NULL)
    {
      team->batsmanHead = newPlayer;
    }
  }
  else if (PlayerRole == 2)
  {
    head = team->bowlersHead;
    if (head == NULL)
    {
      team->bowlersHead = newPlayer;
    }
  }
  else if (PlayerRole == 3)
  {
    head = team->allroundersHead;
    if (head == NULL)
    {
      team->allroundersHead = newPlayer;
    }
  }

  PlayerNode *current = head;
  while (current->next != NULL)
  {
    current = current->next;
  }
  current->next = newPlayer;

  int previousCount = team->TotalPlayers;
  team->TotalPlayers += 1;

  // Recalculating team's Average Batting Strike rate
  if (PlayerRole == 1 || PlayerRole == 2)
  {
    float totalStrikeRate = team->AverageBattingStrikerate * previousCount + StrikeRate;
    team->AverageBattingStrikerate = totalStrikeRate / (float)team->TotalPlayers;
  }

  printf("Player added successfully to team %s", team->Name);
}

void displayTeamPlayers(TeamNode *team)
{
  if (team == NULL)
  {
    printf("Invalid team\n");
    return;
  }

  printf("============================================================================================\n");
  printf("%s %20s %16s %12s %8s %12s %6s %12s\n",
         "ID", "Name", "Role", "Runs", "SR", "Wkts", "ER", "Perf.Index");

  int playerCount = 0;
  PlayerNode *current = team->batsmanHead;
  while (current != NULL)
  {
    displayPlayer(current);
    current = current->next;
    playerCount++;
  }

  current = team->bowlersHead;
  while (current != NULL)
  {
    displayPlayer(current);
    current = current->next;
    playerCount++;
  }

  current = team->allroundersHead;
  while (current != NULL)
  {
    displayPlayer(current);
    current = current->next;
    playerCount++;
  }
  printf("============================================================================================\n");
  printf("Total players: %d\n", team->TotalPlayers);
  printf("Average batting strike rate: %.2f\n", team->AverageBattingStrikerate);
  printf("\n");
}

void displayTeamsByStrikeRate(TeamNode **teamsArray, int teamsCount)
{
  if (teamsArray == NULL)
  {
    printf("Invalid teams array!\n");
    return;
  }

  // Selection sort
  int startingIndex = 0;
  while (startingIndex < teamsCount)
  {
    float maximum = teamsArray[0]->AverageBattingStrikerate;
    int maximumIndex = 0;
    for (int index = startingIndex; index < teamsCount; index++)
    {
      if (teamsArray[index]->AverageBattingStrikerate > maximum)
      {
        maximumIndex = index;
        maximum = teamsArray[index]->AverageBattingStrikerate;
      }
    }
    TeamNode *temp = teamsArray[startingIndex];
    teamsArray[startingIndex] = teamsArray[maximumIndex];
    teamsArray[maximumIndex] = temp;
    startingIndex++;
  }

  printf("Teams sorted by Average batting Strike Rate\n");
  printf("==============================================\n");
  printf(" ID Team Name \tAvg Bat SR \tTotal Players\n");
  printf("==============================================\n");
  for (int index = 0; index < teamsCount; index++)
  {
    printf("%3d %13s %8.2f %8d\n", teamsArray[index]->TeamId, teamsArray[index]->Name, teamsArray[index]->AverageBattingStrikerate, teamsArray[index]->TotalPlayers);
  }
  printf("==============================================\n");
  printf("\n");
}

void displayTopKPlayersOfATeam(TeamNode **teamsArray, int teamsCount)
{
  int teamID;
  int role;
  int playerCount;

  printf("Enter the team ID: ");
  scanf("%d", &teamID);
  if (teamID < 0)
  {
    printf("Invalid team ID\n");
  }

  printf("EnterRole(1-Batsman,2-Bowler,3-All-rounder): ");
  scanf("%d", &role);
  if (role < 1 || role > 3)
  {
    printf("Invalid input\n");
    return;
  }

  printf("Enter number of players: ");
  scanf("%d", &playerCount);
  if (playerCount < 0)
  {
    printf("Invalid input\n");
    return;
  }

  TeamNode *team = searchTeamByID(teamsArray, teamsCount, teamID);
  if (team == NULL)
  {
    printf("Invalid team ID\n");
    return;
  }

  PlayerNode *head = NULL;
  switch (role)
  {
  case 1:
    head = team->batsmanHead;
    break;

  case 2:
    head = team->bowlersHead;
    break;

  case 3:
    head = team->allroundersHead;
    break;

  default:
    printf("Incorrect role entered!\n");
    return;
  }

  printf("Top %d players of Team %s:\n", playerCount, team->Name);
  printf("============================================================================================\n");
  printf("%s %20s %16s %12s %8s %12s %6s %12s\n",
         "ID", "Name", "Role", "Runs", "SR", "Wkts", "ER", "Perf.Index");

  printf("============================================================================================\n");
  for (int index = 0; index < playerCount; index++)
  {
    if (head == NULL)
    {
      break;
    }
    displayPlayer(head);
    head = head->next;
  }
  printf("\n");
}

void DisplayAllPlayersOfRole(TeamNode **teamsArray, int teamsCount)
{
  int role;
  printf("EnterRole(1-Batsman,2-Bowler,3-All-rounder): ");
  scanf("%d", &role);

  if (role < 1 || role > 3)
  {
    printf("Invalid role!\n");
    return;
  }

  printf("============================================================================================\n");
  printf("%s %20s %16s %12s %8s %12s %6s %12s\n",
         "ID", "Name", "Role", "Runs", "SR", "Wkts", "ER", "Perf.Index");

  int *arrayLengths = calloc(teamsCount, sizeof(int));

  PlayerNode **playerArrays = calloc(teamsCount, sizeof(PlayerNode *));
  for (int teamIndex = 0; teamIndex < teamsCount; teamIndex++)
  {
    TeamNode *currentTeam = teamsArray[teamIndex];
    PlayerNode *head = NULL;
    if (role == 1)
    {
      head = currentTeam->batsmanHead;
    }
    else if (role == 2)
    {
      head = currentTeam->bowlersHead;
    }
    else if (role == 3)
    {
      head = currentTeam->allroundersHead;
    }

    int totalPlayers = getLinkedListLength(head);
    arrayLengths[teamIndex] = totalPlayers;
    if (totalPlayers == 0)
    {
      playerArrays[teamIndex] = NULL;
      continue;
    }

    playerArrays[teamIndex] = calloc(totalPlayers, sizeof(PlayerNode));

    // Copy the linked list elements into the player's Arrays
    for (int playerIndex = 0; playerIndex < totalPlayers; playerIndex++)
    {
      if (head == NULL)
      {
        break;
      }
      playerArrays[teamIndex][playerIndex] = *head;
      head = head->next;
    }
  }

  printMergedSortedPlayers(arrayLengths, teamsCount, playerArrays);

  for (int i = 0; i < teamsCount; ++i)
  {
    free(playerArrays[i]);
  }
  free(playerArrays);
  free(arrayLengths);

  printf("\n");
}

void printMenu(TeamNode **teamsArray, int teamsCount)
{
  printf("\n");
  printf("==============================================================================\n");
  printf("ICC ODI Player Performance Analyzer\n");
  printf("==============================================================================\n");
  printf("1. Add Player to Team\n");
  printf("2. Display Players of a Specific Team\n");
  printf("3. Display Teams by Average Batting Strike Rate\n");
  printf("4. Display Top K Players of a Specific Team by Role\n");
  printf("5. Display all Players of specific role Across All Teams by performance index\n");
  printf("6. Exit\n");
  printf("==============================================================================\n");
  printf("Enter your choice: ");

  int choice = 0;
  scanf("%d", &choice);
  printf("\n");

  int teamID = 0;

  switch (choice)
  {
  case 1:
    addPlayer(teamsArray, teamsCount);
    break;
  case 2:
    printf("Enter team ID:");
    scanf("%d", &teamID);
    displayTeamPlayers(searchTeamByID(teamsArray, teamsCount, teamID));
    break;
  case 3:
    displayTeamsByStrikeRate(teamsArray, teamsCount);
    break;
  case 4:
    displayTopKPlayersOfATeam(teamsArray, teamsCount);
    break;
  case 5:
    DisplayAllPlayersOfRole(teamsArray, teamsCount);
    break;
  case 6:
    exit(0);
    break;

  default:
    printf("Incorrect choice, Try Again!\n");
    return;
  }
}

int main()
{
  TeamNode **teamsArray = initializeTeams(teams, teamCount);
  printf("Intialization of teams successful\n");

  initializePlayers(players, playerCount, teamsArray, teamCount);
  setTeamsStrikeRate(teamsArray, teamCount);
  printf("Intialization of players successful\n");

  while (1)
  {
    printMenu(teamsArray, teamCount);
  }
  return 0;
}