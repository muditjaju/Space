/*
 * Asteroid.h
 *
 *  Created on: Feb 19, 2013
 *      Author: ceppelin
 */

#ifndef ASTEROID_H_
#define ASTEROID_H_
#include "cocos2d.h"

class Asteroid : public cocos2d::CCSprite{
private:
	int _lives;
	Asteroid();
	virtual ~Asteroid();
public:
	bool init(const char* asteroidView, int lives);
	static Asteroid* create(const char* asteroidView, int lives);

	void addLife() {++_lives;}
	void subLife() {--_lives;}

	void setLives(int lives) {_lives = lives;}
	int getLives() const {return _lives;}
};

#endif /* ASTEROID_H_ */
