//////////////////////////////////////////////////////////////////////////////////
// Project: Pong
// File:    Main.cpp
//////////////////////////////////////////////////////////////////////////////////

// memory leak
#ifdef _DEBUG
#define  DEBUG_CLIENTBLOCK new(_CLIENT_BLOCK,__FILE__,__LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif // _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

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
#include "../include/Ball.h"
#include "../include/Sticky.h"

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
LTexture gSprite;
SDL_Rect gComputerStickyClip;
SDL_Rect gPlayerStickyClip;
SDL_Rect gBallClip;
int gComputerScore = 0;
int gPlayerScore = 0;
int gBallSpeed = BALL_INIT_SPEED;

Ball* gBall = NULL;// ball and sticky
Sticky* gComputerSticky = NULL;
Sticky* gPlayerSticky = NULL;
bool gStart = true;

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

void GameWin();
void GameLose();

// helper functions
void handleMenuInput();
void handleGameInput();
void handleExitInput();

void handleWinLoseInput();

bool checkWallCollision(Sticky* sticky);
bool checkWallCollision(Ball* ball);
bool checkEntityCollision(Sticky* sticky, Ball *ball);
void changeBallSpeed(Ball* ball);
void changeComputerStickySpeed();

int main(int argc, char** argv) {
	// detect memory leak
	//_CrtSetBreakAlloc(1385);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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

	// load sprite
	gSprite.setColorKey(0xFF, 0, 0xFF);
	if (!gSprite.loadFromFile(gRenderer, "../resources/images/Pong.bmp")) {
		printf("Failed to load background image!\n");
		success = false;
	} else {
		gComputerStickyClip = {COMPUTER_IMG_X,COMPUTER_IMG_Y,STICKY_WIDTH,STICKY_HEIGHT};
		gPlayerStickyClip = { PLAYER_IMG_X,PLAYER_IMG_Y,STICKY_WIDTH,STICKY_HEIGHT };
		gBallClip = {BALL_IMG_X,BALL_IMG_Y,BALL_RADIUS*2,BALL_RADIUS*2};
	}

	return success;
}

void closeSDL() {
	// free texture
	gTextTexture.freeTexture();
	gSprite.freeTexture();

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

	// sticky and ball
	gBall = new Ball(BALL_START_X, BALL_START_Y, &gSprite, &gBallClip);
	gComputerSticky = new Sticky(COMPUTER_START_X, COMPUTER_START_Y, &gSprite, &gComputerStickyClip);
	gPlayerSticky = new Sticky(PLAYER_START_X, PLAYER_START_Y, &gSprite, &gPlayerStickyClip);
}

void shutdown() {
	// deallocate
	delete gBall;
	delete gComputerSticky;
	delete gPlayerSticky;

	gBall = NULL;
	gComputerSticky = NULL;
	gPlayerSticky = NULL;
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
		int textX = (WINDOW_WIDTH - gTextTexture.getWidth()) / 2;
		int textY = (WINDOW_HEIGHT - gTextTexture.getHeight()) / 2;
		gTextTexture.loadFromRenderedText(gRenderer, "Start (G)ame", textColor);
		gTextTexture.render(gRenderer, textX, textY - 10);
		gTextTexture.loadFromRenderedText(gRenderer, "(Q)uit Game", textColor);
		gTextTexture.render(gRenderer, textX, textY + 10);

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

		// player sticky move
		if (!checkWallCollision(gPlayerSticky)) {
			gPlayerSticky->move();
		}
		// computer sticky move
		changeComputerStickySpeed();
		if (!checkWallCollision(gComputerSticky)) {
			gComputerSticky->move();
		}
		// ball move
		/*if (!checkEntityCollision(gPlayerSticky, gBall) && 
			!checkEntityCollision(gComputerSticky, gBall) && 
			!checkWallCollision(gBall)) {
			gBall->move();
		}*/
		changeBallSpeed(gBall);
		gBall->move();

		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		// draw sticky and ball
		gComputerSticky->draw(gRenderer);
		gPlayerSticky->draw(gRenderer);
		gBall->draw(gRenderer);
		// draw score text
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		string scoreText = "Player Score: " + to_string(gPlayerScore)+"   Computer Score: "+to_string(gComputerScore);
		gTextTexture.loadFromRenderedText(gRenderer, scoreText, textColor);
		gTextTexture.render(gRenderer, 0, 0);
		

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
		int textX = (WINDOW_WIDTH - gTextTexture.getWidth()) / 2;
		int textY = (WINDOW_HEIGHT - gTextTexture.getHeight()) / 2;
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, textX, textY);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

void GameWin() {
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleWinLoseInput();
		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		int textX = (WINDOW_WIDTH - gTextTexture.getWidth()) / 2;
		int textY = (WINDOW_HEIGHT - gTextTexture.getHeight()) / 2;
		gTextTexture.loadFromRenderedText(gRenderer, "You Win!!!", textColor);
		gTextTexture.render(gRenderer, textX, textY-10);
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, textX, textY+10);

		// update
		SDL_RenderPresent(gRenderer);
		gTimer = SDL_GetTicks();
	}
}

void GameLose() {
	if ((SDL_GetTicks() - gTimer) >= FRAME_RATE) {
		handleWinLoseInput();
		// clear screen
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(gRenderer);

		// render
		SDL_Color textColor = { 0xFF,0xFF,0xFF };
		int textX = (WINDOW_WIDTH - gTextTexture.getWidth()) / 2;
		int textY = (WINDOW_HEIGHT - gTextTexture.getHeight()) / 2;
		gTextTexture.loadFromRenderedText(gRenderer, "You Lose.", textColor);
		gTextTexture.render(gRenderer, textX, textY - 10);
		gTextTexture.loadFromRenderedText(gRenderer, "Quit Game (Y or N)?", textColor);
		gTextTexture.render(gRenderer, textX, textY+10);

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
			case SDLK_SPACE:
				if (gStart) {
					gStart = false;
					gBall->setVelocity(0, gBallSpeed);
				}
				break;
			case SDLK_LEFT:
				gPlayerSticky->setVelocity(-STICKY_SPEED, 0);
				break;
			case SDLK_RIGHT:
				gPlayerSticky->setVelocity(STICKY_SPEED, 0);
				break;
			default:
				break;
			}
		}
		if (gEvent.type == SDL_KEYUP) {
			switch (gEvent.key.keysym.sym)
			{
			case SDLK_LEFT:
				gPlayerSticky->setVelocity(0, 0);
				break;
			case SDLK_RIGHT:
				gPlayerSticky->setVelocity(0, 0);
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

void handleWinLoseInput() {
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
				temp.StatePointer = Exit;
				gStageStack.push(temp);
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

bool checkWallCollision(Sticky* sticky) {
	int left = sticky->getStartX();
	int right = sticky->getStartX() + sticky->getWidth();
	// check
	left += sticky->getVelocityX();
	right += sticky->getVelocityX();
	if (left < GAME_AREA_LEFT || right > GAME_AREA_RIGHT) {
		return true;
	}
	return false;
}

bool checkWallCollision(Ball* ball) {
	int x = ball->getCenterX() + ball->getVelocityX();
	int y = ball->getCenterY() + ball->getVelocityY();
	int radius = ball->getRadius();
	if (x - radius<GAME_AREA_LEFT || x + radius>GAME_AREA_RIGHT || y - radius<GAME_AREA_TOP || y + radius>GAME_AREA_BOTTOM) {
		return true;
	}
	return false;
}

bool checkEntityCollision(Sticky* sticky, Ball *ball) {
	// get ball position
	int ballX = ball->getCenterX()+ball->getVelocityX();
	int ballY = ball->getCenterY()+ball->getVelocityY();
	int radius = ball->getRadius();
	// get closest point to ball on sticky
	int stickyX = ballX;
	int stickyY = ballY;
	int tempX = sticky->getStartX();
	int tempY = sticky->getStartY();
	if (ballX < tempX) {
		stickyX = tempX;
	}
	if (ballY < tempY) {
		stickyY = tempY;
	}
	tempX += sticky->getWidth();
	tempY += sticky->getHeight();
	if (ballX > tempX) {
		stickyX = tempX;
	}
	if (ballY > tempY) {
		stickyY = tempY;
	}

	// count distance square
	tempX = stickyX - ballX;
	tempY = stickyY - ballY;
	if (tempX*tempX + tempY*tempY < radius*radius) {
		return true;
	}
	return false;
}

void changeBallSpeed(Ball* ball) {
	int velX = ball->getVelocityX();
	int velY = ball->getVelocityY();
	int x = ball->getCenterX();
	int y = ball->getCenterY();
	int radius = ball->getRadius();
	// change speed when collision with wall
	if (x - radius < GAME_AREA_LEFT || x + radius > GAME_AREA_RIGHT) {
		ball->setVelocity(-velX, velY);
	}
	// check game win or lose score
	if (y + radius < 0) {
		// computer get score
		gPlayerScore++;
		// reset ball and start flag
		gBall->setCenter(BALL_START_X, BALL_START_Y);
		gBall->setVelocity(0, 0);
		gBallSpeed = BALL_INIT_SPEED;
		gStart = true;

		if (gPlayerScore >= SCORE) {
			// lose game
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			// reset
			gComputerScore = 0;
			gPlayerScore = 0;
			// game win state
			StateStruct win;
			win.StatePointer = GameWin;
			gStageStack.push(win);
		}
	}
	if (y - radius > WINDOW_HEIGHT) {
		// player get score
		gComputerScore++;
		// reset ball and start flag
		gBall->setCenter(BALL_START_X, BALL_START_Y);
		gBall->setVelocity(0, 0);
		gBallSpeed = BALL_INIT_SPEED;
		gStart = true;

		if (gComputerScore >= SCORE) {
			// lose game
			while (!gStageStack.empty()) {
				gStageStack.pop();
			}
			// reset
			gComputerScore = 0;
			gPlayerScore = 0;
			// game lose state
			StateStruct lose;
			lose.StatePointer = GameLose;
			gStageStack.push(lose);
		}
	}
	// change speed when collision with sticky
	if (checkEntityCollision(gPlayerSticky, ball)) {
		if (x < gPlayerSticky->getStartX() || 
			x > gPlayerSticky->getStartX() + gPlayerSticky->getWidth()) {
			gBall->setVelocity(-velX, velY);
		} else {
			// set speed
			velY += BALL_CHANGE_SPEED;
			gBall->setVelocity(gPlayerSticky->getVelocityX() + velX, -velY);
			// set position
			gBall->setCenter(x + velX, gPlayerSticky->getStartY() - radius);

		}
	}
	if (checkEntityCollision(gComputerSticky, ball)) {
		if (x < gComputerSticky->getStartX() ||
			x > gComputerSticky->getStartX() + gComputerSticky->getWidth()) {
			gBall->setVelocity(-velX, velY);
		} else {
			velY -= BALL_CHANGE_SPEED;
			gBall->setVelocity(gComputerSticky->getVelocityX() + velX, -velY);
			// set position
			gBall->setCenter(x + velX, gComputerSticky->getStartY() + gComputerSticky->getHeight() + radius);
		}
	}
}

void changeComputerStickySpeed() {
	if (gBall->getVelocityY() < 0 && 
		gBall->getCenterY()>gComputerSticky->getStartY()+gComputerSticky->getHeight()) {
		// count computer sticky left and right
		int left = gComputerSticky->getStartX();
		int right = gComputerSticky->getStartX() + gComputerSticky->getWidth();
		// get ball center x
		int ballX = gBall->getCenterX();
		if (ballX <= left) {
			gComputerSticky->setVelocity(-STICKY_SPEED, 0);
		}
		else if (ballX >= right) {
			gComputerSticky->setVelocity(STICKY_SPEED, 0);
		} else {
			gComputerSticky->setVelocity(0, 0);
		}
	} else {
		gComputerSticky->setVelocity(0, 0);
	}
}