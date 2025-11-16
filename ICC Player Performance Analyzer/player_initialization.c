#include "ICC_header.h"

PlayerNode *insertPlayerByPerformanceIndex(PlayerNode *head, PlayerNode *newPlayer)
{
  if (head == NULL)
  {
    return newPlayer;
  }

  PlayerNode *previous = NULL;
  PlayerNode *current = head;
  while (current != NULL)
  {
    if (newPlayer->PerformanceIndex > current->PerformanceIndex)
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

  team->TotalPlayers += 1;

  if (strcmp(player->Role, "Batsman") == 0)
  {
    team->batsmanHead = insertPlayerByPerformanceIndex(team->batsmanHead, player);
    return;
  }
  else if (strcmp(player->Role, "Bowler") == 0)
  {
    team->bowlersHead = insertPlayerByPerformanceIndex(team->bowlersHead, player);
    return;
  }
  else if (strcmp(player->Role, "All-rounder") == 0)
  {
    team->allroundersHead = insertPlayerByPerformanceIndex(team->allroundersHead, player);
    return;
  }
}

TeamNode **initializeTeams(const char **teamNames, int teamsCount)
{
  TeamNode **allTeams = calloc(teamsCount, sizeof(TeamNode *));

  for (int teamID = 0; teamID < teamsCount; teamID++)
  {

    TeamNode *newTeam = malloc(sizeof *newTeam);
    newTeam->TeamId = teamID;
    strncpy(newTeam->Name, teamNames[teamID], MAX_NAME);
    newTeam->TotalPlayers = 0;
    newTeam->AverageBattingStrikerate = 0;
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
    newPlayer->PlayerId = playerDetails[index].id;
    strncpy(newPlayer->Name, playerDetails[index].name, MAX_NAME);
    strncpy(newPlayer->TeamName, playerDetails[index].team, MAX_NAME);
    strncpy(newPlayer->Role, playerDetails[index].role, MAX_ROLE_NAME);
    newPlayer->TotalRuns = playerDetails[index].totalRuns;
    newPlayer->BattingAverage = playerDetails[index].battingAverage;
    newPlayer->StrikeRate = playerDetails[index].strikeRate;
    newPlayer->Wickets = playerDetails[index].wickets;
    newPlayer->EconomyRate = playerDetails[index].economyRate;
    newPlayer->PerformanceIndex = calculatePerformanceIndex(newPlayer->Role, newPlayer->BattingAverage, newPlayer->StrikeRate, newPlayer->Wickets, newPlayer->EconomyRate);
    newPlayer->next = NULL;

    int teamIndex = binarySearchTeamsName(teamsArray, teamsCount, newPlayer->TeamName);

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
      totalStrikeRate += playersHead->StrikeRate;
      playersHead = playersHead->next;
      allRounderBatsmenCount++;
    }

    playersHead = currentTeam->allroundersHead;
    while (playersHead != NULL)
    {
      totalStrikeRate += playersHead->StrikeRate;
      playersHead = playersHead->next;
      allRounderBatsmenCount++;
    }
    currentTeam->AverageBattingStrikerate = totalStrikeRate / allRounderBatsmenCount;
  }
}
