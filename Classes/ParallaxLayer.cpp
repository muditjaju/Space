/*
 * ParallaxLayer.cpp
 *
 *  Created on: Feb 16, 2013
 *      Author: ceppelin
 */

#include "/home/ceppelin/Desktop/cocos2d-2.1beta3-x-2.1.1/cocosbox/Classes/ParallaxLayer.h"

class CCPointObject : CCObject{
	CC_SYNTHESIZE(CCPoint, m_tRatio, Ratio)
	CC_SYNTHESIZE(CCPoint, m_tOffset, Offset)
	CC_SYNTHESIZE(CCNode *,m_pchild, Child)
};
ParallaxLayer::ParallaxLayer() {
	CCParallaxNode();
}

ParallaxLayer::~ParallaxLayer() {
}

ParallaxLayer* ParallaxLayer::node(){
	return new ParallaxLayer();
}
void ParallaxLayer::incrementOffset(CCPoint offset, CCNode *node){
	for(uint i = 0; i < m_pParallaxArray->num; ++i){
		CCPointObject *point = (CCPointObject*) m_pParallaxArray->arr[i];
		CCNode* curNode = point->getChild();
		if (curNode->isEqual(node)){
			point->setOffset(ccpAdd(point->getOffset(), offset));
			break;
		}
	}
}



