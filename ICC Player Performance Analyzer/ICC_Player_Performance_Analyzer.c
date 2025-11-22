#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ICC_header.h"

void addPlayer(TeamNode **teamsArray, int teamsCount)
{
  int teamID;
  int playerID;
  char playerName[MAX_NAME];
  int playerRole;
  int totalRuns;
  float battingAverage;
  float strikeRate;
  int wickets;
  float economyRate;

  printf("Enter team ID: ");
  scanf("%d", &teamID);
  if (teamID < 1 || teamID > MAX_TEAM_ID)
  {
    printf("Invalid team ID\n");
    return;
  }
  TeamNode *team = searchTeamByID(teamsArray, teamsCount, teamID);
  if (team == NULL)
  {
    printf("Team with id %d not found\n", teamID);
    return;
  }
  if (team->totalPlayers >= MAX_PLAYERS_PER_TEAM)
  {
    printf("Team full, can't add more players!\n");
    return;
  }

  printf("Enter player details:\n");

  printf("Enter player ID: ");
  scanf("%d", &playerID);
  if (findPlayerByID(teamsArray, teamsCount, playerID) == 1)
  {
    printf("This player ID already exists\n");
    return;
  }

  printf("Enter player name: ");
  getchar();
  if (scanf("%[^\n]", playerName) != 1)
  {
    printf("Invalid name\n");
    return;
  }

  printf("Enter player role(1-Batsman,2-Bowler,3-All Rounder): ");
  scanf("%d", &playerRole);
  if (playerRole < 1 || playerRole > 3)
  {
    printf("Invalid Role\n");
    return;
  }
  char roleName[MAX_ROLE_NAME];
  if (playerRole == 1)
  {
    strncpy(roleName, "Batsman", MAX_ROLE_NAME);
    roleName[MAX_ROLE_NAME - 1] = '\0';
  }
  else if (playerRole == 2)
  {
    strncpy(roleName, "Bowler", MAX_ROLE_NAME);
    roleName[MAX_ROLE_NAME - 1] = '\0';
  }
  else if (playerRole == 3)
  {
    strncpy(roleName, "All-rounder", MAX_ROLE_NAME);
    roleName[MAX_ROLE_NAME - 1] = '\0';
  }

  printf("Enter total runs: ");
  scanf("%d", &totalRuns);
  if (totalRuns < 0)
  {
    printf("Invalid runs\n");
    return;
  }

  printf("Enter batting average: ");
  scanf("%f", &battingAverage);
  if (battingAverage < 0)
  {
    printf("Invalid batting average\n");
    return;
  }

  printf("Enter strike rate: ");
  scanf("%f", &strikeRate);
  if (strikeRate < 0)
  {
    printf("Invalid strike rate\n");
    return;
  }

  printf("Enter total wickets: ");
  scanf("%d", &wickets);
  if (wickets < 0)
  {
    printf("wickets less than zero\n");
    return;
  }

  printf("Enter economy rate: ");
  scanf("%f", &economyRate);
  if (economyRate < 0)
  {
    printf("Invalid economy rate\n");
    return;
  }

  PlayerNode *newPlayer = malloc(sizeof *newPlayer);
  if (newPlayer == NULL)
  {
    printf("Memory allocation failed\n");
    return;
  }
  newPlayer->next = NULL;
  newPlayer->playerID = playerID;
  strncpy(newPlayer->name, playerName, MAX_NAME);
  newPlayer->name[MAX_NAME - 1] = '\0';
  strncpy(newPlayer->teamName, team->name, MAX_NAME);
  newPlayer->teamName[MAX_NAME - 1] = '\0';
  strncpy(newPlayer->role, roleName, MAX_ROLE_NAME);
  newPlayer->role[MAX_ROLE_NAME - 1] = '\0';
  newPlayer->totalRuns = totalRuns;
  newPlayer->battingAverage = battingAverage;
  newPlayer->strikeRate = strikeRate;
  newPlayer->wickets = wickets;
  newPlayer->economyRate = economyRate;
  newPlayer->performanceIndex = calculatePerformanceIndex(newPlayer->role, newPlayer->battingAverage, newPlayer->strikeRate, newPlayer->wickets, newPlayer->economyRate);

  insertPlayerIntoTeam(team, newPlayer);

  int previousCount = team->totalPlayers;
  team->totalPlayers += 1;

  // Recalculating team's Average Batting Strike rate
  if (playerRole == 1 || playerRole == 3)
  {
    float totalStrikeRate = team->averageBattingStrikerate * previousCount + strikeRate;
    team->averageBattingStrikerate = totalStrikeRate / (float)team->totalPlayers;
  }

  printf("Player added successfully to team %s", team->name);
}

void displayTeamPlayers(TeamNode *team)
{
  if (team == NULL)
  {
    printf("Invalid team\n");
    return;
  }

  printf("============================================================================================\n");
  printf("%s %20s  %12s  %5s  %8s  %8s  %4s  %3s  %s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");

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
  printf("Total players: %d\n", team->totalPlayers);
  printf("Average batting strike rate: %.2f\n", team->averageBattingStrikerate);
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
    float maximum = teamsArray[startingIndex]->averageBattingStrikerate;
    int maximumIndex = startingIndex;
    for (int index = startingIndex; index < teamsCount; index++)
    {
      if (teamsArray[index]->averageBattingStrikerate > maximum)
      {
        maximumIndex = index;
        maximum = teamsArray[index]->averageBattingStrikerate;
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
    printf("%3d %13s %8.2f %8d\n", teamsArray[index]->teamID, teamsArray[index]->name, teamsArray[index]->averageBattingStrikerate, teamsArray[index]->totalPlayers);
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
  if (teamID < 1)
  {
    printf("Invalid team ID\n");
    return;
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
    printf("Invalid role entered!\n");
    return;
  }

  printf("Top %d players of Team %s:\n", playerCount, team->name);
  printf("============================================================================================\n");
  printf("%s %20s  %12s  %5s  %8s  %8s  %4s  %3s  %s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");

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

void displayAllPlayersOfRole(TeamNode **teamsArray, int teamsCount)
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
  printf("%s %20s  %12s  %5s  %8s  %8s  %4s  %3s  %s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");

  int *arrayLengths = calloc(teamsCount, sizeof(int));
  if (arrayLengths == NULL)
  {
    printf("Memory allocation failed\n");
    return;
  }

  PlayerNode **playerArrays = calloc(teamsCount, sizeof(PlayerNode *));
  if (playerArrays == NULL)
  {
    printf("Memory allocation failed\n");
    return;
  }
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
    if (playerArrays[teamIndex] == NULL)
    {
      printf("Memory allocation failed\n");
      return;
    }

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

void freeMemoryAndExit(TeamNode **teamsArray, int teamsCount)
{
  for (int teamIndex = 0; teamIndex < teamCount; teamIndex++)
  {
    free(teamsArray[teamIndex]);
  }
  free(teamsArray);
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
    displayAllPlayersOfRole(teamsArray, teamsCount);
    break;
  case 6:
    freeMemoryAndExit(teamsArray, teamCount);
    break;

  default:
    printf("Invalid choice, Try Again!\n");
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

  for (int teamIndex = 0; teamIndex < teamCount; teamIndex++)
  {
    free(teamsArray[teamIndex]);
  }
  free(teamsArray);
  return 0;
}