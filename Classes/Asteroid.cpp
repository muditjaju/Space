/*
 * Asteroid.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: ceppelin
 */

#include "/home/ceppelin/Desktop/cocos2d-2.1beta3-x-2.1.1/cocosbox/Classes/Asteroid.h"

Asteroid::Asteroid() {
	_lives = 3;
}

Asteroid::~Asteroid() {}

bool Asteroid::init(const char* asteroidView, int lives) {
	_lives = lives;
	return initWithSpriteFrameName(asteroidView);
}
Asteroid* Asteroid::create(const char* asteroidView, int lives){
	Asteroid* instance = new Asteroid();
	if (instance && instance->init(asteroidView, lives)){
		instance->autorelease();
		return instance;
	} else {
		delete instance;
		instance = NULL;
		return NULL;
	}
}
