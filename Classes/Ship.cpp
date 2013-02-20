/*
 * Ship.cpp
 *
 *  Created on: Feb 16, 2013
 *      Author: ceppelin
 */

#include "/home/ceppelin/Desktop/cocos2d-2.1beta3-x-2.1.1/cocosbox/Classes/Ship.h"

Ship::Ship() {
	cage = 0; // # of bullet
	_score = 0;
	_lives = 3;
	bullets = new std::vector<CCSprite*>();
}
Ship::~Ship() {
	delete bullets;
	bullets = NULL;
}

bool Ship::init(const char* shipView, float speed/*, WeaponType weaponType*/) {
	_speed = speed;
	//_weaponType = weaponType; // 2nd init (1st in setWeapon)
	return initWithSpriteFrameName(shipView);
}

Ship* Ship::create(const char* shipView, float speed, /*WeaponType weaponType,*/
				   const char* weaponView, CCNode *parent){
	Ship* instance = new Ship();
	if (instance && instance->init(shipView, speed/* weaponType*/)){
		instance->autorelease();

		instance->setWeapon(weaponView/*, weaponType*/, parent);

		return instance;
	} else {
		delete instance;
		instance = NULL;
		return NULL;
	}
}
void Ship::setWeapon(const char* weapon/*, WeaponType weaponType*/, CCNode *layer){
	if (!bullets->empty()){
		for(int i = 0; i < bullets->size(); i++)
			layer->removeChild(bullets->at(i), true);
		bullets->clear();
	}
	int k = 1; // for diff weapon. next step !
	for(int i = 0; i < 5*k; i++){
		CCSprite *laser = CCSprite::createWithSpriteFrameName(weapon);
		laser->setTag(2);
		laser->setVisible(false);
		layer->addChild(laser, 1);
		bullets->push_back(laser);
	}
}
void Ship::setInvisible(CCNode * node){
	node->setVisible(false);
}
void Ship::shoot(){
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	CCSprite *laser = bullets->at(cage++);
	if (cage >= bullets->size())
		cage = 0;
	if (!laser->isVisible()) {
		laser->setPosition(ccpAdd(this->getPosition(), ccp(laser->getContentSize().width/2, 0)));
		laser->setVisible(true);
		laser->stopAllActions();
		laser->runAction(CCSequence::create(CCMoveBy::create(0.5, ccp(winSize.width, 0)),
						 CCCallFuncN::create(this, callfuncN_selector(Ship::setInvisible)),NULL));
	}

}
