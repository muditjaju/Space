/*
 * ParallaxLayer.h
 *
 *  Created on: Feb 16, 2013
 *      Author: ceppelin
 */

#ifndef PARALLAXLAYER_H_
#define PARALLAXLAYER_H_

#include <cocos2d.h>
USING_NS_CC;
class ParallaxLayer : public CCParallaxNode {
public:
	ParallaxLayer();
	virtual ~ParallaxLayer();

	static ParallaxLayer* node();
	void incrementOffset(CCPoint offset, CCNode *node);
};

#endif /* PARALLAXLAYER_H_ */
