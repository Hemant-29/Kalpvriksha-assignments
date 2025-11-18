#include "ICC_header.h"

int binarySearchTeamsName(TeamNode **teamsArray, int teamsCount, char *teamName)
{
  int low = 0;
  int high = teamsCount - 1;

  while (low <= high)
  {
    int mid = (high - low) / 2 + low;

    if (strcmp(teamsArray[mid]->name, teamName) == 0)
    {
      return mid;
    }
    else if (strcmp(teamsArray[mid]->name, teamName) < 0)
    {
      low = mid + 1;
    }
    else if (strcmp(teamsArray[mid]->name, teamName) > 0)
    {
      high = mid - 1;
    }
  }
  return -1;
}

float calculatePerformanceIndex(char role[MAX_NAME], float battingAverage, float strikeRate, int wickets, float economyRate)
{
  float index = 0;
  if (strcmp(role, "Batsman") == 0)
  {
    index = (battingAverage * strikeRate) / 100;
  }
  else if (strcmp(role, "Bowler") == 0)
  {
    index = (wickets * 2) + (100 - economyRate);
  }
  else if (strcmp(role, "All-rounder") == 0)
  {
    index = ((battingAverage * strikeRate) / 100) + (wickets * 2);
  }
  else
  {
    printf("Incorrect role\n");
    return -1;
  }
  return index;
}

TeamNode *searchTeamByID(TeamNode **teamsArray, int totalTeams, int teamID)
{
  if (teamsArray == NULL)
  {
    return NULL;
  }
  if (teamID < 1)
  {
    printf("Invalid team ID\n");
    return NULL;
  }
  TeamNode *team = NULL;

  for (int index = 0; index < totalTeams; index++)
  {
    if (teamsArray[index]->teamID == teamID)
    {
      team = teamsArray[index];
      return team;
    }
  }

  return team;
}

void displayPlayer(PlayerNode *player)
{
  printf("%4d %20s  %12s  %5d  %8.2f  %8.2f  %4d  %3.2f  %.2f\n", player->playerID, player->name, player->role, player->totalRuns, player->battingAverage, player->strikeRate, player->wickets, player->economyRate, player->performanceIndex);
}

int getLinkedListLength(PlayerNode *head)
{
  int length = 0;
  while (head != NULL)
  {
    length++;
    head = head->next;
  }
  return length;
}

int findPlayerByID(TeamNode **teamsArray, int teamsCount, int id)
{
  if (teamsArray == NULL)
  {
    return 0;
  }

  for (int teamIndex = 0; teamIndex < teamsCount; teamIndex++)
  {
    TeamNode *team = teamsArray[teamIndex];
    PlayerNode *head = team->batsmanHead;
    while (head != NULL)
    {
      if (head->playerID == id)
      {
        return 1;
      }
      head = head->next;
    }
    head = team->bowlersHead;
    while (head != NULL)
    {
      if (head->playerID == id)
      {
        return 1;
      }
      head = head->next;
    }
    head = team->allroundersHead;
    while (head != NULL)
    {
      if (head->playerID == id)
      {
        return 1;
      }
      head = head->next;
    }
  }
  return 0;
}