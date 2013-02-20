/*
 * PauseDialog.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: ceppelin
 */

#include "/home/ceppelin/Desktop/cocos2d-2.1beta3-x-2.1.1/cocosbox/Classes/PauseDialog.h"

bool PauseDialog::isActive = false;

PauseDialog* PauseDialog::create(CCNode *caller){

	PauseDialog* instance = new PauseDialog();

	if (instance && instance->init(caller)){
		instance->autorelease();
		return instance;
	} else {
		delete instance;
		instance = NULL;
		return NULL;
	}
}
bool PauseDialog::init(CCNode *caller){
	if (!CCLayer::init())
		return false;
	_caller = caller;
	//isActive = false;
	return true;
}

void PauseDialog::pauseDialog(CCNode* background, bool isLoose, const char *title){
	int center_width  = CCDirector::sharedDirector()->getVisibleSize().width  / 2;
	int center_height = CCDirector::sharedDirector()->getVisibleSize().height / 2;
	if (background) {
		background->setPosition(ccp(center_width  - background->getPositionX()/2,
									center_height - background->getPositionY()/2));
		this->addChild(background);
	}
	CCLabelTTF *labelTitle = CCLabelTTF::create(title, "Arial", 34);
	labelTitle->setPosition(ccp(center_width, center_height+40));
	this->addChild(labelTitle);

	CCLabelTTF *labelRefresh    = CCLabelTTF::create("restart",      "Arial", 22);
	CCLabelTTF *labelBackToMain = CCLabelTTF::create("main menu",    "Arial", 22);
	CCLabelTTF *labelBack       = CCLabelTTF::create("back",         "Arial", 22);

	CCMenuItem *menuItemRefresh     = CCMenuItemLabel::create(labelRefresh,    this, menu_selector(PauseDialog::onClickMenuRefresh));
	CCMenuItem *menuItemBack        = CCMenuItemLabel::create(labelBack,       this, menu_selector(PauseDialog::onClickMenuBack));
	CCMenuItem *menuItemBackToMain  = CCMenuItemLabel::create(labelBackToMain, this, menu_selector(PauseDialog::onClickMenuBackToMain));
	CCMenu *menuRefresh    = CCMenu::create(menuItemRefresh   , NULL);
	CCMenu *menuBack       = CCMenu::create(menuItemBack      , NULL);
	CCMenu *menuBackToMain = CCMenu::create(menuItemBackToMain, NULL);

	menuRefresh   ->setPosition(ccp(center_width, center_height+15));
	menuBackToMain->setPosition(ccp(center_width, center_height-15));
	menuBack      ->setPosition(ccp(center_width, center_height-30));

	this->addChild(menuRefresh);
	this->addChild(menuBackToMain);
	if (!isLoose) // if game over: back to main or new game
		this->addChild(menuBack);

}

void PauseDialog::onClickMenuRefresh(CCObject* pSender){
	isActive = false;
}
void PauseDialog::onClickMenuBack(CCObject* pSender){
	this->removeAllChildrenWithCleanup(true);
	_caller->resumeSchedulerAndActions();
	isActive = false;
}
void PauseDialog::onClickMenuBackToMain(CCObject* pSender){
	CCDirector::sharedDirector()->popScene();
	isActive = false;
}
