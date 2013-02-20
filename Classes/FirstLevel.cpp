/*
 * FirstLevel.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: ceppelin
 */

#include "/home/ceppelin/Desktop/cocos2d-2.1beta3-x-2.1.1/cocosbox/Classes/FirstLevel.h"


using namespace cocos2d;

CCScene* FirstLevel::scene()
{
    CCScene *scene = CCScene::create();
    FirstLevel *layer = FirstLevel::create();
    scene->addChild(layer);
    return scene;
}

bool FirstLevel::init() {
    if (!CCLayer::init())
        return false;
    _nextAsteroid      = 0;
    _nextAsteroidSpawn = 0;
    _shipPositionY     = 0;
    score = 10;

    _asteroids   = new std::vector<Asteroid*>();
    _b2Asteroids = new std::vector<b2Body*>();
    _b2Bullets   = new std::vector<b2Body*>();

    winSize = CCDirector::sharedDirector()->getWinSize();
/*close button*/
    CCMenuItemImage *closeItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(FirstLevel::menuCloseCallback) );
    closeItem->setPosition(ccp(winSize.width - 20, 20));
    CCMenu* menu = CCMenu::create(closeItem, NULL);
    menu->setPosition(CCPointZero);
    this->addChild(menu, 5);

    _batchNode = CCSpriteBatchNode::create("Sprites.pvr.ccz");
    this->addChild(_batchNode);

    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
/*ship*/
    _ship = Ship::create("SpaceFlier_sm_1.png", 0.05, "laserbeam_blue.png", _batchNode);
    _ship->setPosition(ccp(winSize.width *0.1, winSize.height * 0.5));
    _ship->setTag(0);

    _batchNode->addChild(_ship, 2);
/*parallax*/
    _background = ParallaxLayer::node();
    this->addChild(_background, -1);

    _spacedust1      = CCSprite::create("bg_front_spacedust.png");
    _spacedust2      = CCSprite::create("bg_front_spacedust.png");

    CCPoint dustSpeed = ccp(0.1, 0.1);

    _background->addChild(_spacedust1, 0, dustSpeed, ccp(0, winSize.height/2));
    _background->addChild(_spacedust2, 0, dustSpeed, ccp(_spacedust1->getContentSize().width, winSize.height/2));
    this->addChild(CCParticleSystemQuad::create("Stars1.plist"), 3);
    this->addChild(CCParticleSystemQuad::create("Stars3.plist"), 3);
    this->addChild(CCParticleSystemQuad::create("Stars2.plist"), 3);
/*world*/
    b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
    _world = new b2World(gravity);
    _collisionListener = new CollisionsListener();
    _world->SetContactListener(_collisionListener);

    b2BodyDef ground;
    ground.position.Set(0, 0);
    b2Body *groundBody = _world->CreateBody(&ground);

    b2EdgeShape groundEdge;
    b2FixtureDef boxShapeDef;
    boxShapeDef.shape = &groundEdge;
    groundEdge.Set(b2Vec2(0, 0), b2Vec2(winSize.width/PTM_RATIO, 0));
    groundBody->CreateFixture(&boxShapeDef);

    _shipBody = createBody(_ship,
    					   1.0f,
    					   0.0f,
    					   0.0f,
    					   _ship->getPosition().x,
    					   _ship->getPosition().y,
    					   79.0f,
    					   42.0f);
/*asteroids*/
    for(int i = 0; i < 15; ++i) {
        Asteroid* asteroid = Asteroid::create("asteroid.png", 3);
        float randScale = randomBetween(0.3, 1.0);
        asteroid->setScale  (randScale);
        asteroid->setVisible(false);
        asteroid->setTag(1);

        _batchNode->addChild (asteroid);
        _asteroids->push_back(asteroid);
        b2Body *ast = createBody(asteroid,
        						 0.0f,
        						 0.0f,
        						 0.0f,
        						 winSize.width + asteroid->getContentSize().width,
        						 0.0f,
        						 (55.0*randScale));
        _b2Asteroids->push_back(ast);
    }
/*bullets (lasers)
 * bullets sprites create in Ship class
 * they stores in ship.bullets*/
    for(int i = 0; i < _ship->bullets->size(); ++i){
    	b2Body *bullet = createBody(_ship->bullets->at(i),
    							  1.0f,
    							  0.0f,
    							  0.0f,
    							  _ship->bullets->at(i)->getPosition().x,
    							  _ship->bullets->at(i)->getPosition().y,
    							  20.0f,
    							  3.0f);
    	_b2Bullets->push_back(bullet);
    }
/*debug GL*/
    if (DEBUG) {
        _debugDraw = new GLESDebugDraw(PTM_RATIO);
        _world->SetDebugDraw(_debugDraw);
        uint32 flags = 0;
        flags += b2Draw::e_shapeBit;
        _debugDraw->SetFlags(flags);
    }

    this->showLives(_ship);
    this->showScore(_ship);

    this->setTouchEnabled(true);
    this->scheduleUpdate();
    this->setAccelerometerEnabled(true);

    return true;
}

void FirstLevel::setInvisible(CCNode* node){
	node->setVisible(false);
}

b2Body* FirstLevel::createBody(CCNode* object,
									   float density,
									   float friction,
									   float restitution,
									   float x,
									   float y,
									   float radius){
	b2Body *temp;
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(x/PTM_RATIO, y/PTM_RATIO);
	ballBodyDef.userData = object;
	temp = _world->CreateBody(&ballBodyDef);

	b2CircleShape circle;
	circle.m_radius = radius / PTM_RATIO;

	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;
	ballShapeDef.density = density;
	ballShapeDef.friction = friction;
	ballShapeDef.restitution = restitution;
	temp->CreateFixture(&ballShapeDef);
	return temp;
}
b2Body* FirstLevel::createBody(CCNode* object,
									   float density,
									   float friction,
									   float restitution,
									   float x,
									   float y,
									   float width,
									   float height){
	b2Body *temp;
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(x/PTM_RATIO, y/PTM_RATIO);
	ballBodyDef.userData = object;
	temp = _world->CreateBody(&ballBodyDef);

	b2PolygonShape shape;
	shape.SetAsBox(width/2/PTM_RATIO, height/2/PTM_RATIO);

	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &shape;
	ballShapeDef.density = density;
	ballShapeDef.friction = friction;
	ballShapeDef.restitution = restitution;
	temp->CreateFixture(&ballShapeDef);
	return temp;
}

void FirstLevel::draw(){
	if (DEBUG) {
		glDisable(GL_TEXTURE_2D);
		_world->DrawDebugData();
		glEnable(GL_TEXTURE_2D);
	}
}

void FirstLevel::didAccelerate(CCAcceleration* pAccelerationValue){
	#define KFILTERINGFACTOR 0.1
	#define KRESTACCELX -0.3
	#define KSHIPMAXPOINTSPERSEC (winSize.height*0.5)
	#define KMAXDIFFX 0.2

	double rollingX = 0;

	// Cocos2DX inverts X and Y accelerometer depending on device orientation
	// in landscape mode right x=-y and y=x !!! (Strange and confusing choice)
	pAccelerationValue->x = pAccelerationValue->y ;
	rollingX = (pAccelerationValue->x * KFILTERINGFACTOR) + (rollingX * (1.0 - KFILTERINGFACTOR));
	float accelX = pAccelerationValue->x - rollingX ;
	float accelDiff = accelX - KRESTACCELX;
	float accelFraction = accelDiff / KMAXDIFFX;
	_shipPositionY = KSHIPMAXPOINTSPERSEC * accelFraction;
}

void FirstLevel::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event){
	_ship->shoot();
}

void FirstLevel::menuCloseCallback(CCObject* pSender)
{
    //CCDirector::sharedDirector()->end();
	if (!PauseDialog::isActive){
		PauseDialog::isActive = true;
		PauseDialog *dialog = PauseDialog::create(this);
		dialog->pauseDialog(NULL, false, "pause");
		this->addChild(dialog);

		this->pauseSchedulerAndActions();
		for(int i = 0; i < _asteroids->size(); ++i){
			_asteroids->at(i)->pauseSchedulerAndActions(); // !! stops asteroids
		}
	}
}

/* low + something; where something is value between 0 and high-low. (s.o.)*/
float FirstLevel::randomBetween(float low, float high){
	float random = ((float) rand() / (float) RAND_MAX);
	float diff = high - low;
	float r = random * diff;
	return low + r;
}

float FirstLevel::getTimeTick(){ // get time in milliseconds
	timeval time;
	gettimeofday(&time, NULL);
	unsigned long ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (float) ms;
}

void FirstLevel::showLives(Ship* ship) {
	labelLives = CCLabelTTF::create(toString("Lives", _ship->getLives()).c_str(), "Arial", 15);
	labelLives->setPosition(ccp(400, 300));
	this->addChild(labelLives);
}

void FirstLevel::showScore(Ship* ship) {
	labelScore = CCLabelTTF::create(toString("Score", _ship->getScore()).c_str(), "Arial", 15);
	labelScore->setPosition(ccp(400, 285));
	this->addChild(labelScore);
}

void FirstLevel::updateLives(Ship* ship) {
	labelLives->setString(toString("Lives", ship->getLives()).c_str());
}

void FirstLevel::updateScore(Ship* ship) {
	labelScore->setString(toString("Score", _ship->getScore()).c_str());
}

template<class T> string FirstLevel::toString(char* message, T t){
	stringstream ss;
	ss<<message<<": "<<t;
	return ss.str();
}

void FirstLevel::update(float dt) {
/*parallax */
	CCPoint backgroundScrollVert = ccp(-1000,0) ;
	_background->setPosition(ccpAdd(_background->getPosition(),ccpMult(backgroundScrollVert,dt)));

	CCArray *spaceDusts = CCArray::createWithCapacity(2);
	spaceDusts->addObject(_spacedust1) ;
	spaceDusts->addObject(_spacedust2) ;
	for (int i = 0; i < spaceDusts->count(); i++ ) {
		CCSprite * spaceDust = (CCSprite *)(spaceDusts->objectAtIndex(i)) ;
		float xPosition = _background->convertToWorldSpace(spaceDust->getPosition()).x;
		float size = spaceDust->getContentSize().width ;
		if (xPosition < -size)
			_background->incrementOffset(ccp(spaceDust->getContentSize().width*2,0),spaceDust);
	}
/*end parallax */
/*accelerometer*/
	float maxY = winSize.height - _ship->getContentSize().height/2;
	float minY = _ship->getContentSize().height/2;
	float diff = _shipPositionY * dt;
	float newY = _ship->getPosition().y + diff;

	newY = MIN(MAX(newY, minY), maxY);
	_ship->setPosition(ccp(_ship->getPosition().x, newY));
/*end accelerometer*/

/*first we move sprites and then bodies*/
/*sprites animation*/
	float curTime = getTimeTick();

	if (curTime > _nextAsteroidSpawn){                   // if time to show new asteroid?
		float randY  = randomBetween(0, winSize.height); // new asteroid position
		float randms = randomBetween(0.2, 1.0)*1000;     // how often show new asteroid

		_nextAsteroidSpawn = randms + curTime;		     // when show next asteroid
		float duration = randomBetween(1.0, 7.0);		 // android's "speed"

		Asteroid* asteroid = _asteroids->at(_nextAsteroid++);
		if(_nextAsteroid >= _asteroids->size())
			_nextAsteroid = 0;
		/*take asteroid from cage and set next params;
		 *when asteroid end moving - hide */
			asteroid->stopAllActions();
			asteroid->setPosition(ccp(winSize.width + asteroid->getContentSize().width/2, randY));
			asteroid->setLives(3);
			asteroid->setVisible(true);
			asteroid->runAction(CCSequence::create(CCMoveBy::create(duration,ccp( -winSize.width - asteroid->getContentSize().width, 0)),
								 	 	           CCCallFuncN::create(this, callfuncN_selector(FirstLevel::setInvisible)),NULL));
	}
/*end sprites animation*/

/*bodies animation*/
	_world->Step(dt, 10, 10);
	for(b2Body *b = _world->GetBodyList(); b; b = b->GetNext()){
		if (b->GetUserData() != NULL){
			CCSprite* sprite = (CCSprite*) b->GetUserData();
			if (sprite->isVisible()) {
				b2Vec2 position = b2Vec2(sprite->getPosition().x/PTM_RATIO,
									sprite->getPosition().y/PTM_RATIO);
				float angle = -1 * CC_DEGREES_TO_RADIANS(sprite->getRotation());
				b->SetTransform(position, angle);
			}
		}
	}
/*end bodies animation*/

/*body's collisions*/
	std::deque<CurrentContact>::iterator i;
	for(i = _collisionListener->contacts.begin(); i != _collisionListener->contacts.end(); ++i){
		CurrentContact contact = *i;
		b2Body *a = contact.a->GetBody();
		b2Body *b = contact.b->GetBody();

		if (a->GetUserData() != NULL && b->GetUserData() != NULL){
			CCSprite *sA = (CCSprite*)a->GetUserData();
			CCSprite *sB = (CCSprite*)b->GetUserData();

			Asteroid *ast;
			CCSprite *bullet;
			if (sA->isVisible() && sB->isVisible()) {
/*asteroid and bullet (laser)*/
				//if (sA->getTag() != 0 || sB->getTag() != 0)
				if ((sA->getTag() == 1 && sB->getTag() == 2) ||
					(sA->getTag() == 2 && sB->getTag() == 1)) {
					b2Body *temp; // bullet body
					if (sA->getTag() == 1) {
						ast    = (Asteroid*) a->GetUserData();
						bullet = (CCSprite*) b->GetUserData();
						temp   = b;
					} else {
						ast    = (Asteroid*) b->GetUserData();
						bullet = (CCSprite*) a->GetUserData();
						temp   = a;
					}
					if (ast->getLives() > 1){  // hide only bullet
						ast->subLife();
						temp->SetTransform(b2Vec2(-100.0, -100.0), 0.0f); // bullet body
					} else { // hide asteroid and bullet
						a->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
						b->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
						ast->stopAllActions();
						ast->setVisible(false);

						_ship->addScore(score);
						updateScore(_ship);
					}
					bullet->stopAllActions();
					bullet->setVisible(false);
					continue;
				}
/*asteroid and ship*/
				if ((sA->getTag() == 0 && sB->getTag() == 1) ||
					(sA->getTag() == 1 && sB->getTag() == 0)) {
					if (_ship->getLives() > 0){
						_ship->subLife();
						updateLives(_ship);
						_ship->runAction(CCBlink::create(1.0, 3));
						if (sA->getTag() == 1){
							sA->setVisible(false);
							sA->stopAllActions();
							a->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
						} else {
							sB->setVisible(false);
							sB->stopAllActions();
							b->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
						}

					} else {
						CCLog("GAME OVER");
						// TODO: show dialog
					}
					continue;
				}
			}
		}
	}
/*end body's collisions*/
}

