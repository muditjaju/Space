/*
 * FirstLevel.h
 *
 *  Created on: Feb 19, 2013
 *      Author: ceppelin
 */

#ifndef FIRSTLEVEL_H_
#define FIRSTLEVEL_H_

#define DEBUG false
#define PTM_RATIO 32

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "Ship.h"
#include "ParallaxLayer.h"
#include "Asteroid.h"
#include "PauseDialog.h"
#include "CollisionsListener.h"
#include "Box2D/GLES-Render.h"

class FirstLevel : public CCLayer{
private:
	/* Tags:
	 * ship:           0
	 * asteroid:       1
	 * bullet(laser) : 2 */

	b2World       *_world;
	GLESDebugDraw *_debugDraw;

	Ship   *_ship;
	b2Body *_shipBody;

	CCSpriteBatchNode *_batchNode;
	ParallaxLayer     *_background;

	CCSprite *_spacedust1;
	CCSprite *_spacedust2;

	CollisionsListener *_collisionListener;

	CCLabelTTF *labelLives;
	CCLabelTTF *labelScore;

	std::vector<Asteroid*> *_asteroids;
	std::vector<b2Body*>   *_b2Asteroids;

	/*bullets sprites inside Ship class*/
	std::vector<b2Body*>   *_b2Bullets;

	float _shipPositionY; // accelerometer value

	float _nextAsteroidSpawn;
	int   _nextAsteroid;

public:
	int score;
	CCSize winSize;
	float randomBetween(float low, float high);
	float getTimeTick();                        // get current time in ms

	/*create b2Body: circle or box*/
	b2Body* createBody(CCNode *object,
					   float density,
					   float friction,
					   float restitution,
					   float xPosition,
					   float yPosition,
					   float radius);
	b2Body* createBody(CCNode *object,
					   float density,
					   float friction,
					   float restitution,
					   float xPosition,
					   float yPosition,
					   float width,
					   float height);

	void setInvisible(CCNode* node);
	void showScore   (Ship* ship);
	//void hideScore   (Ship* ship);
	void updateScore (Ship* ship);
	void showLives   (Ship* ship);
	//void hideLives   (Ship* ship);
	void updateLives (Ship* ship);
	template<class T> string toString(char* message,T num); // num to string

	static  CCScene* scene();
	virtual bool init();
	virtual void didAccelerate(CCAcceleration* pAccelerationValue);
			void draw();                               // GL debug
			void menuCloseCallback(CCObject* pSender); // exit button
			void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
			void update(float dt);
	CREATE_FUNC(FirstLevel);
};

#endif /* FIRSTLEVEL_H_ */
