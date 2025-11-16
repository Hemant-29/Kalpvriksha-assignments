#include "ICC_header.h"


int binarySearchTeamsName(TeamNode **teamsArray, int teamsCount, char *teamName)
{
  int low = 0;
  int high = teamsCount - 1;

  while (low <= high)
  {
    int mid = (high - low) / 2 + low;

    if (strcmp(teamsArray[mid]->Name, teamName) == 0)
    {
      return mid;
    }
    else if (strcmp(teamsArray[mid]->Name, teamName) < 0)
    {
      low = mid + 1;
    }
    else if (strcmp(teamsArray[mid]->Name, teamName) > 0)
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
  if (teamID < 0)
  {
    return NULL;
  }
  TeamNode *team = NULL;

  for (int index = 0; index < totalTeams; index++)
  {
    if (teamsArray[index]->TeamId == teamID)
    {
      team = teamsArray[index];
    }
  }

  return team;
}

void displayPlayer(PlayerNode *player)
{
  printf("%d %20s \t%12s \t%d \t%8.2f \t%d \t%3.2f \t%.2f\n", player->PlayerId, player->Name, player->Role, player->TotalRuns, player->StrikeRate, player->Wickets, player->EconomyRate, player->PerformanceIndex);
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
      if (head->PlayerId == id)
      {
        return 1;
      }
      head = head->next;
    }
    head = team->bowlersHead;
    while (head != NULL)
    {
      if (head->PlayerId == id)
      {
        return 1;
      }
      head = head->next;
    }
    head = team->allroundersHead;
    while (head != NULL)
    {
      if (head->PlayerId == id)
      {
        return 1;
      }
      head = head->next;
    }
  }
  return 0;
}