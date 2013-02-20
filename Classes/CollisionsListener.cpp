/*
 * CollisionsListener.cpp
 *
 *  Created on: Feb 18, 2013
 *      Author: ceppelin
 */

#include "/home/ceppelin/Desktop/cocos2d-2.1beta3-x-2.1.1/cocosbox/Classes/CollisionsListener.h"

CollisionsListener::CollisionsListener(): contacts() {
}

CollisionsListener::~CollisionsListener() {
}

void CollisionsListener::BeginContact(b2Contact* contact) {

	CurrentContact current = {contact->GetFixtureA(), contact->GetFixtureB()};
	contacts.push_back(current);

	//cocos2d::CCLog("collision detected");
}
void CollisionsListener::EndContact(b2Contact* contact) {
	CurrentContact current = {contact->GetFixtureA(), contact->GetFixtureB()};
	std::deque<CurrentContact>::iterator i;
	i = std::find(contacts.begin(), contacts.end(), current);
	if (i != contacts.end())
		contacts.erase(i);

	//cocos2d::CCLog("collision end");
}
void CollisionsListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {}
void CollisionsListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {}
