#include "ICC_header.h"

PlayerNode *insertPlayerByPerformanceIndex(PlayerNode *head, PlayerNode *newPlayer)
{
  if (newPlayer == NULL)
  {
    return head;
  }

  if (head == NULL)
  {
    head = newPlayer;
    return head;
  }

  PlayerNode *previous = NULL;
  PlayerNode *current = head;
  while (current != NULL)
  {
    if (newPlayer->performanceIndex > current->performanceIndex)
    {
      if (previous == NULL)
      {
        newPlayer->next = current;
        return newPlayer;
      }
      else
      {
        newPlayer->next = previous->next;
        previous->next = newPlayer;
        return head;
      }
    }
    previous = current;
    current = current->next;
  }
  // If all nodes been traversed and no greater performance index found
  previous->next = newPlayer;
  return head;
}

void insertPlayerIntoTeam(TeamNode *team, PlayerNode *player)
{

  if (team == NULL)
  {
    printf("Invalid team\n");
    return;
  }
  if (player == NULL)
  {
    printf("Invalid player\n");
    return;
  }

  if (strcmp(player->role, "Batsman") == 0)
  {
    team->batsmanHead = insertPlayerByPerformanceIndex(team->batsmanHead, player);
    team->totalPlayers += 1;
    return;
  }
  else if (strcmp(player->role, "Bowler") == 0)
  {
    team->bowlersHead = insertPlayerByPerformanceIndex(team->bowlersHead, player);
    team->totalPlayers += 1;
    return;
  }
  else if (strcmp(player->role, "All-rounder") == 0)
  {
    team->allroundersHead = insertPlayerByPerformanceIndex(team->allroundersHead, player);
    team->totalPlayers += 1;
    return;
  }
  else
  {
    printf("player role Invalid\n");
    return;
  }
}

TeamNode **initializeTeams(const char **teamNames, int teamsCount)
{
  TeamNode **allTeams = calloc(teamsCount, sizeof(TeamNode *));
  if (allTeams == NULL)
  {
    printf("Memory allocation failed\n");
    return NULL;
  }

  for (int teamID = 0; teamID < teamsCount; teamID++)
  {

    TeamNode *newTeam = malloc(sizeof *newTeam);
    if (newTeam == NULL)
    {
      printf("Memory allocation failed\n");
      return NULL;
    }
    newTeam->teamID = teamID + 1; // for 1 based indexing
    strncpy(newTeam->name, teamNames[teamID], MAX_NAME);
    newTeam->totalPlayers = 0;
    newTeam->averageBattingStrikerate = 0;
    newTeam->batsmanHead = NULL;
    newTeam->bowlersHead = NULL;
    newTeam->allroundersHead = NULL;

    allTeams[teamID] = newTeam;
  }

  return allTeams;
}

void initializePlayers(const Player *playerDetails, int playersCount, TeamNode **teamsArray, int teamsCount)
{
  for (int index = 0; index < playersCount; index++)
  {
    PlayerNode *newPlayer = malloc(sizeof *newPlayer);
    if (newPlayer == NULL)
    {
      printf("Memory allocation failed\n");
      return;
    }
    newPlayer->playerID = playerDetails[index].id;
    strncpy(newPlayer->name, playerDetails[index].name, MAX_NAME);
    strncpy(newPlayer->teamName, playerDetails[index].team, MAX_NAME);
    strncpy(newPlayer->role, playerDetails[index].role, MAX_ROLE_NAME);
    newPlayer->totalRuns = playerDetails[index].totalRuns;
    newPlayer->battingAverage = playerDetails[index].battingAverage;
    newPlayer->strikeRate = playerDetails[index].strikeRate;
    newPlayer->wickets = playerDetails[index].wickets;
    newPlayer->economyRate = playerDetails[index].economyRate;
    newPlayer->performanceIndex = calculatePerformanceIndex(newPlayer->role, newPlayer->battingAverage, newPlayer->strikeRate, newPlayer->wickets, newPlayer->economyRate);
    newPlayer->next = NULL;

    int teamIndex = binarySearchTeamsName(teamsArray, teamsCount, newPlayer->teamName);

    if (teamIndex >= 0)
    {
      insertPlayerIntoTeam(teamsArray[teamIndex], newPlayer);
    }
  }
}

void setTeamsStrikeRate(TeamNode **teamsArray, int teamsCount)
{
  for (int index = 0; index < teamsCount; index++)
  {
    TeamNode *currentTeam = teamsArray[index];
    float totalStrikeRate = 0;
    float allRounderBatsmenCount = 0;

    PlayerNode *playersHead = currentTeam->batsmanHead;
    while (playersHead != NULL)
    {
      totalStrikeRate += playersHead->strikeRate;
      playersHead = playersHead->next;
      allRounderBatsmenCount++;
    }

    playersHead = currentTeam->allroundersHead;
    while (playersHead != NULL)
    {
      totalStrikeRate += playersHead->strikeRate;
      playersHead = playersHead->next;
      allRounderBatsmenCount++;
    }
    if (allRounderBatsmenCount > 0)
    {
      currentTeam->averageBattingStrikerate = totalStrikeRate / allRounderBatsmenCount;
    }
    else
    {
      currentTeam->averageBattingStrikerate = 0.0f;
    }
    // currentTeam->averageBattingStrikerate = totalStrikeRate / allRounderBatsmenCount;
  }
}
