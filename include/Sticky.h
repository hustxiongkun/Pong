//////////////////////////////////////////////////////////////////////////
// Sticky.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "../include/Tools.h"

class Sticky
{
public:
	Sticky();
	// constructor
	Sticky(int x, int y, LTexture* texture, SDL_Rect* clip, int velX = 0, int velY = 0);

	// move
	void move();

	// draw
	void draw(SDL_Renderer* renderer);

	// set center position
	void setStart(int x, int y);

	// set velocity
	void setVelocity(int x, int y);

	// getter
	int getStartX();
	int getStartY();
	int getVelocityX();
	int getVelocityY();
	int getWidth();
	int getHeight();

	~Sticky();

private:
	// start position
	int mStartX;
	int mStartY;
	// velocity
	int mVelocityX;
	int mVelocityY;

	// texture
	LTexture* mTexture;
	SDL_Rect* mClip;
};

Sticky::Sticky()
{
}

Sticky::~Sticky()
{
}

Sticky::Sticky(int x, int y, LTexture* texture, SDL_Rect* clip, int velX, int velY) :
	mStartX(x), mStartY(y), mTexture(texture), mClip(clip), mVelocityX(velX), mVelocityY(velY) {

}

void Sticky::move() {
	mStartX += mVelocityX;
	mStartY += mVelocityY;
}

void Sticky::draw(SDL_Renderer* renderer) {
	mTexture->render(renderer, mStartX, mStartY, mClip);
}

void Sticky::setStart(int x, int y) {
	mStartX = x;
	mStartY = y;
}

void Sticky::setVelocity(int x, int y) {
	mVelocityX = x;
	mVelocityY = y;
}

int Sticky::getStartX() {
	return mStartX;
}

int Sticky::getStartY() {
	return mStartY;
}

int Sticky::getVelocityX() {
	return mVelocityX;
}

int Sticky::getVelocityY() {
	return mVelocityY;
}

int Sticky::getWidth() {
	return mClip->w;
}

int Sticky::getHeight() {
	return mClip->h;
}