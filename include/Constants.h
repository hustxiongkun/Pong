//////////////////////////////////////////////////////////////////////////
// Constants.h
//////////////////////////////////////////////////////////////////////////

#pragma once

// constant data
// window setting
const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 400;
const char* WINDOW_CAPTION = "Pong";

// game setting
const int FRAMES_PER_SECOND = 30;
const int FRAME_RATE = 1000/FRAMES_PER_SECOND;
// game area setting
const int GAME_AREA_LEFT = 0;
const int GAME_AREA_RIGHT = WINDOW_WIDTH;
const int GAME_AREA_BOTTOM = WINDOW_HEIGHT;
const int GAME_AREA_TOP = 20;
const int SCORE = 5;

// sprite image related
const int COMPUTER_IMG_X = 0;
const int COMPUTER_IMG_Y = 0;
const int PLAYER_IMG_X = 0;
const int PLAYER_IMG_Y = 16;
const int BALL_IMG_X = 68;
const int BALL_IMG_Y = 1;
// dimensions
const int STICKY_WIDTH = 60;
const int STICKY_HEIGHT = 15;
const int BALL_RADIUS = 7;
// object start coordinate
const int COMPUTER_START_X = (WINDOW_WIDTH - STICKY_WIDTH) / 2;
const int COMPUTER_START_Y = GAME_AREA_TOP;
const int PLAYER_START_X = (WINDOW_WIDTH - STICKY_WIDTH) / 2;
const int PLAYER_START_Y = (GAME_AREA_BOTTOM - STICKY_HEIGHT);
const int BALL_START_X = WINDOW_WIDTH / 2;
const int BALL_START_Y = (GAME_AREA_TOP+GAME_AREA_BOTTOM) / 2;
// speed
const int BALL_INIT_SPEED = 5;
const int BALL_CHANGE_SPEED = 1;
const int STICKY_SPEED = 5;	
