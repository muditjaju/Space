#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#define PTM_RATIO 32

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "Ship.h"
#include "ParallaxLayer.h"
#include "Asteroid.h"
#include "CollisionsListener.h"
#include "Box2D/GLES-Render.h"

class HelloWorld : public cocos2d::CCLayer
{
private:
	b2World *_world;
	GLESDebugDraw *_debugDraw;
	Ship *_ship;          // ship - :public CCSprite
	b2Body *_shipBody;
	CCSpriteBatchNode *_batchNode;
	ParallaxLayer *_background;
	CCSprite *_spacedust1;
	CCSprite *_spacedust2;
	CollisionsListener *_collisionListener;
	CCLabelTTF *labelLives;
	CCLabelTTF *labelScore;

	std::vector<Asteroid*> *_asteroids;
	std::vector<b2Body*> *_b2Asteroids;
	std::vector<b2Body*> *_b2Bullets;

	/*accel*/
	float _shipPositionY;
	/*asteroids*/
	float _nextAsteroidSpawn;
	int   _nextAsteroid;

public:
    virtual bool init();  

	void  update(float dt);
	float randomBetween(float low, float high);
	float getTimeTick();
	/* coord without box2d metrics; without PTM_RATIO !*/
	b2Body* createb2CircleBody(CCNode* object, float density, float friction,
					   	   	   float restitution, float x_position, float y_position, float radius);
	void setPositionb2Body(b2Body* body, float x, float y); // TODO: remove!
	void setInvisible(CCNode* node);
	void draw();
	template<class T> string toString(T t);
	void showScore(Ship* ship);
	void hideScore(Ship* ship);
	void updateScore(Ship* ship);
	void showLives(Ship* ship);
	void hideLives(Ship* ship);
	void updateLives(Ship* ship);
	void pauseMenu(bool isLoose); // TODO: create pause menu
    void menuCloseCallback(CCObject* pSender);

    void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);

    static cocos2d::CCScene* scene();

    virtual void didAccelerate(CCAcceleration* pAccelerationValue);

    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
