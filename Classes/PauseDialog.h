/*
 * PauseDialog.h
 *
 *  Created on: Feb 20, 2013
 *      Author: ceppelin
 */

#ifndef PAUSEDIALOG_H_
#define PAUSEDIALOG_H_

#include "cocos2d.h"
USING_NS_CC;

class PauseDialog : public CCLayer{
private:
	CCNode* _caller;
public:

	static bool isActive;

	static PauseDialog* create(CCNode *caller);
		   bool         init(CCNode *caller);
		   void         pauseDialog(CCNode* parent, bool isLoose, const char *title);

protected:
		   void onClickMenuBack      (CCObject* pSender);
		   void	onClickMenuRefresh   (CCObject* pSender);
		   void	onClickMenuBackToMain(CCObject* pSender);

};

#endif /* PAUSEDIALOG_H_ */
