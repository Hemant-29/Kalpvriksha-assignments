#ifndef ICC_HEADER_H
#define ICC_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define MAX_NAME 51
#define MAX_ROLE_NAME 12
#define MAX_PLAYERS_PER_TEAM 50
#define MAX_TEAM_ID 1000

typedef struct PlayerNode
{
  int playerID;
  char name[MAX_NAME];
  char teamName[MAX_NAME];
  char role[MAX_ROLE_NAME];
  int totalRuns;
  float battingAverage;
  float strikeRate;
  int wickets;
  float economyRate;
  float performanceIndex;
  struct PlayerNode *next;
} PlayerNode;

typedef struct TeamNode
{
  int teamID;
  char name[MAX_NAME];
  int totalPlayers;
  float averageBattingStrikerate;

  PlayerNode *batsmanHead;
  PlayerNode *bowlersHead;
  PlayerNode *allroundersHead;
} TeamNode;

typedef struct HeapNode
{
  PlayerNode *data;
  int array;
  int index;
} HeapNode;

void swapNodes(HeapNode *first, HeapNode *second);

void insertInMaxHeap(HeapNode *heap, int *size, int capacity, HeapNode node);

void deleteFromMaxHeap(HeapNode *heap, int *size);

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