//////////////////////////////////////////////////////////////////////////
// Game.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <stack>

#include "../include/Tools.h"

// game state struct
struct StateStruct{
	void(*StatePointer)();
};

// constant data
// window setting
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const char* WINDOW_CAPTION = "GameName";

// game setting
const unsigned int FRAMES_PER_SECOND = 30;
const unsigned int FRAME_RATE = 1000/FRAMES_PER_SECOND;

// global data
std::stack<StateStruct> gStageStack; // stack for game state pointer
SDL_Window* gWindow = NULL; // SDL window pointer
SDL_Renderer* gRenderer = NULL; // renderer pointer
SDL_Event gEvent; // SDL event struct
int gTimer; // timer
LTexture gTextTexture;// texture for text

// init and close SDL, load media
bool initSDL();
bool loadMedia();
void closeSDL();

// init and shutdown game
void init();
void shutdown();

// functions to handle states of the game
void Menu();
void Game();
void Exit();

// helper functions
void handleMenuInput();
void handleGameInput();
void handleExitInput();