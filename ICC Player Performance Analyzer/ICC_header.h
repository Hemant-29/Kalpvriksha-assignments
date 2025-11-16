#ifndef Header
#define Header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define MAX_NAME 51
#define MAX_ROLE_NAME 12
#define MAX_PLAYERS_PER_TEAM 50

typedef struct PlayerNode
{
  int PlayerId;
  char Name[MAX_NAME];
  char TeamName[MAX_NAME];
  char Role[MAX_ROLE_NAME];
  int TotalRuns;
  float BattingAverage;
  float StrikeRate;
  int Wickets;
  float EconomyRate;
  float PerformanceIndex;
  struct PlayerNode *next;
} PlayerNode;

typedef struct TeamNode
{
  int TeamId;
  char Name[MAX_NAME];
  int TotalPlayers;
  float AverageBattingStrikerate;

  PlayerNode *batsmanHead;
  PlayerNode *bowlersHead;
  PlayerNode *allroundersHead;
} TeamNode;

typedef struct heapNode
{
  PlayerNode *data;
  int array;
  int index;
} heapNode;

void swapNodes(heapNode *first, heapNode *second);

void insertInMinHeap(heapNode *heap, int *size, int capacity, heapNode node);

void deleteFromMinHeap(heapNode *heap, int *size);

void printMergedSortedPlayers(int *arrayLengths, int totalArrays, PlayerNode **arrays);

int binarySearchTeamsName(TeamNode **teamsArray, int teamsCount, char *teamName);

float calculatePerformanceIndex(char role[MAX_NAME], float battingAverage, float strikeRate, int wickets, float economyRate);

TeamNode *searchTeamByID(TeamNode **teamsArray, int totalTeams, int teamID);

void displayPlayer(PlayerNode *player);

void printAllTeams(TeamNode **teamsArray, int teamsCount);

int getLinkedListLength(PlayerNode *head);

int findPlayerByID(TeamNode **teamsArray, int teamsCount, int id);

PlayerNode *insertPlayerByPerformanceIndex(PlayerNode *head, PlayerNode *newPlayer);

void insertPlayerIntoTeam(TeamNode *team, PlayerNode *player);

TeamNode **initializeTeams(const char **teamNames, int teamsCount);

void initializePlayers(const Player *playerDetails, int playersCount, TeamNode **teamsArray, int teamsCount);

void setTeamsStrikeRate(TeamNode **teamsArray, int teamsCount);

#endif