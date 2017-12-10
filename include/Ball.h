//////////////////////////////////////////////////////////////////////////
// Ball.h
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "../include/Tools.h"

class Ball
{
public:
	Ball();
	// constructor
	Ball(int x, int y, LTexture* texture, SDL_Rect* clip, int radius = 0, int velX = 0, int velY = 0);

	// move
	void move();

	// draw
	void draw(SDL_Renderer* renderer);

	// set center position
	void setCenter(int x, int y);

	// set velocity
	void setVelocity(int x, int y);

	// getter
	int getCenterX();
	int getCenterY();
	int getVelocityX();
	int getVelocityY();
	int getRadius();

	~Ball();

private:
	// center position
	int mCenterX;
	int mCenterY;
	// velocity
	int mVelocityX;
	int mVelocityY;

	// dimension
	int mRadius;

	// texture
	LTexture* mTexture;
	SDL_Rect* mClip;
};

Ball::Ball()
{
}

Ball::~Ball()
{
}

Ball::Ball(int x, int y, LTexture* texture, SDL_Rect* clip, int radius, int velX, int velY):
	mCenterX(x),mCenterY(y),mTexture(texture),mClip(clip),mRadius(radius),mVelocityX(velX),mVelocityY(velY){
	if (radius == 0) {
		mRadius = clip->w / 2;
	}
}

void Ball::move() {
	mCenterX += mVelocityX;
	mCenterY += mVelocityY;
}

void Ball::draw(SDL_Renderer* renderer) {
	mTexture->render(renderer, mCenterX - mRadius, mCenterY - mRadius, mClip);
}

void Ball::setCenter(int x, int y) {
	mCenterX = x;
	mCenterY = y;
}

void Ball::setVelocity(int x, int y) {
	mVelocityX = x;
	mVelocityY = y;
}

int Ball::getCenterX() {
	return mCenterX;
}

int Ball::getCenterY() {
	return mCenterY;
}

int Ball::getVelocityX() {
	return mVelocityX;
}

int Ball::getVelocityY() {
	return mVelocityY;
}

int Ball::getRadius() {
	return mRadius;
}