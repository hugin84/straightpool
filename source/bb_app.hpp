#pragma once

#include <stdint.h>
#include <sys/types.h>

enum BB_CURRENT_PLAYER
{
	  BB_PLAYER_1
	, BB_PLAYER_2
	, BB_NUM_PLAYERS
};


enum BB_GAME_STAGE
{
	  BB_GAME_STAGE_SETUP
	, BB_GAME_STAGE_BREAK
	, BB_GAME_STAGE_PLAY
	, BB_GAME_STAGE_STATS
};


enum
{
	BB_MAX_PLAYER_NAME_LEN = 128
};


typedef struct bb_player
{
	char name[BB_MAX_PLAYER_NAME_LEN];
	int  score;
	uint num_consecutive_fouls;
	uint num_fouls;
	int  highrun;
	int  nationality;
	int  num_innings;
} bb_player;


typedef struct bb_game_state
{
	bb_player players[BB_NUM_PLAYERS];
	uint      target_score;
	uint      inning_start_balls;
	uint      balls_on_table;
	uint      current_player;
	uint      game_stage;
} bb_game_state;


void bb_app_run();
