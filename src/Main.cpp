//////////////////////////////////////////////////////////////////////////////////
// Project: Game Framework
// File:    Main.cpp
//////////////////////////////////////////////////////////////////////////////////

// links
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_ttf.lib")
#pragma comment(lib, "SDL2_image.lib")

#include <cstdio>
#include <ctime>

#include <stack>

#include "../include/Constants.h"
#include "../include/Tools.h"

using namespace std;

// game state struct
struct StateStruct{
	void(*StatePointer)();
};

// global data
std::stack<StateStruct> gStageStack; // stack for game state pointer
SDL_Window* gWindow = NULL; // SDL window pointer
SDL_Renderer* gRenderer = NULL; // renderer pointer
SDL_Event gEvent; // SDL event struct
int gTimer; // timer
LTexture gTextTexture;// texture for text

// functions
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

int main(int argc, char** argv) {

	// start up SDL and create window
	if (!initSDL()) {
		printf("Failed to initialize!\n");
	} else {
		// load media
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			// game
			init();
			// main loop
			while (!gStageStack.empty()) {
				gStageStack.top().StatePointer();
			}

			shutdown();
		}
	}

	// free resources and close SDL
	closeSDL();

	return 0;
}

bool initSDL() {
	// initialization flag
	bool success = true;

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		// create window
		gWindow = SDL_CreateWindow(WINDOW_CAPTION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			// create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				// initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);

				// initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				// initialize SDL_ttf
				if (TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	// loading success flag
	bool success = true;

	gTextTexture.setFont("../resources/fonts/ARIAL.TTF", 12);
	//gTextTexture.setFont("../../resources/fonts/ARIAL.TTF", 12);

	return success;
}

void closeSDL() {
	// free texture
	gTextTexture.free();

	// destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void init() {
	// get the number of ticks
	gTimer = SDL_GetTicks();

	// seed our random number generator
	srand(time(0));
	
	// add a pointer to exit state
	StateStruct state;
	state.StatePointer = Exit;
	gStageStack.push(state);

	// add a pointer to menu state
	state.StatePointer = Menu;
	gStageStack.push(state);
}

void shutdown() {
	// deallocate
}

// game menu
void Menu() {
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		// handle input
		handleMenuInput();

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		gTextTexture.loadFromRenderedText(gRenderer, "Start (G)ame", textColor);
		gTextTexture.render(gRenderer, 350, 250);
		gTextTexture.loadFromRenderedText(gRenderer, "(Q)uit Game", textColor);
		gTextTexture.render(gRenderer, 350, 270);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// main game
void Game() {
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleGameInput();

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// exit state
void Exit() {
	// control FPS
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleExitInput();
		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, 350, 250);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

// receive input handle it for menu state
void handleMenuInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
			case SDLK_g:
				StateStruct temp;
				temp.StatePointer = Game;// add a pointer to game state
				gStageStack.push(temp);
				return;// this state is done, exit the function
				break;
			default:
				break;
			}
		}
	}
}

// receive input handle it for main game
void handleGameInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
			default:
				break;
			}
		}
	}
}

// receive input handle it for exit state
void handleExitInput() {
	// get event information
	while (SDL_PollEvent(&gEvent) != 0) {
		// handle user manually closing game window
		if (gEvent.type == SDL_QUIT) {
			// pop all state
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			return;// game is over, exit the function
		}
		// handle keyboard input
		if (gEvent.type == SDL_KEYDOWN) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				gStageStack.pop();
				return;// this state is done, exit the function
				break;
			// Yes
			case SDLK_y:
				gStageStack.pop();
				return;// game is over, exit the function
				break;
			// No
			case SDLK_n:
				StateStruct temp;
				temp.StatePointer = Menu;
				gStageStack.push(temp);
				return;// this state is done, exit the function
				break;
			default:
				break;
			}
		}
	}
}