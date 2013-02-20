/*
 * CollisionsListener.h
 *
 *  Created on: Feb 18, 2013
 *      Author: ceppelin
 */

#ifndef COLLISIONSLISTENER_H_
#define COLLISIONSLISTENER_H_
#include "Box2D/Box2D.h"
#include "cocos2d.h"
#include <deque>
class CurrentContact{
public:
	b2Fixture *a;
	b2Fixture *b;
	bool operator==(const CurrentContact& newContact) const {
		return (a == newContact.a && b == newContact.b);
	}
};
class CollisionsListener : public b2ContactListener{
public:
	std::deque<CurrentContact> contacts;

	CollisionsListener();
	virtual ~CollisionsListener();

	virtual void BeginContact(b2Contact* contact);
	virtual void EndContact(b2Contact* contact);
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
};

#endif /* COLLISIONSLISTENER_H_ */
