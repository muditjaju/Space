/*
 * Ship.h
 *
 *  Created on: Feb 16, 2013
 *      Author: ceppelin
 */

#ifndef SHIP_H_
#define SHIP_H_
#include <vector>
#include <cocos2d.h>
USING_NS_CC;

/*num WeaponType{
	SINGLE_LASER, DOUBLE_LASER
};*/

class Ship : public CCSprite{
private:
	float      _speed;
	//WeaponType _weaponType;
	int        _lives;
	long       _score;

	Ship();
	virtual ~Ship();
public:
	int cage;
	std::vector<CCSprite*> *bullets;

	bool init(const char* shipView, float speed/*, WeaponType weaponType*/);
	static Ship* create(const char* shipView, float speed/* WeaponType weaponType*/,
						const char* weaponView, CCNode* parentlayer);

	void setWeapon(const char* weaponView/* WeaponType weaponType*/, CCNode* parent);
	void shoot();

	void addLife(){++_lives;}
	void subLife(){--_lives;}
	void addScore(int a){_score+=a;}
	void subScore(int a){_score-=a;}
	void setInvisible(CCNode * node);
	//WeaponType getWeaponType() const {return _weaponType;}
	float getSpeed() const {return _speed;}
	int   getLives() const {return _lives;}
	long  getScore() const {return _score;}

};

#endif /* SHIP_H_ */
