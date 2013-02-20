#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#define DEBUG true
using namespace cocos2d;
using namespace CocosDenshion;

CCScene* HelloWorld::scene()
{
    CCScene *scene = CCScene::create();
    HelloWorld *layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if (!CCLayer::init()){
        return false;
    }
    CCSize winsize = CCDirector::sharedDirector()->getWinSize();
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    _batchNode = CCSpriteBatchNode::create("Sprites.pvr.ccz");
    this->addChild(_batchNode);

    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Sprites.plist");
    _ship = Ship::create("SpaceFlier_sm_1.png", 0.05, "laserbeam_blue.png", _batchNode);
    _ship->setPosition(ccp(winsize.width *0.1, winsize.height * 0.5));
    _ship->setTag(0);

    _batchNode->addChild(_ship, 2);
/*parallax*/
    _background = ParallaxLayer::node();
    this->addChild(_background, -1);

    _spacedust1      = CCSprite::create("bg_front_spacedust.png");
    _spacedust2      = CCSprite::create("bg_front_spacedust.png");

    CCPoint dustSpeed = ccp(0.1, 0.1);

    _background->addChild(_spacedust1, 0, dustSpeed, ccp(0, winsize.height/2));
    _background->addChild(_spacedust2, 0, dustSpeed, ccp(_spacedust1->getContentSize().width, winsize.height/2));
    this->addChild(CCParticleSystemQuad::create("Stars1.plist"), 3);
    this->addChild(CCParticleSystemQuad::create("Stars3.plist"), 3);
    this->addChild(CCParticleSystemQuad::create("Stars2.plist"), 3);
  /*box*/

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
    groundEdge.Set(b2Vec2(0, 0), b2Vec2(winsize.width/PTM_RATIO, 0));
    groundBody->CreateFixture(&boxShapeDef);

    _shipBody = createb2CircleBody(_ship, 1.0f, 0.2f, 0.8f, 100, 300, 25.0);

    /*asteroids*/
    _nextAsteroid = 0;
    _nextAsteroidSpawn = 0;
    _shipPositionY = 0;

    _asteroids   = new std::vector<Asteroid*>();
    _b2Asteroids = new std::vector<b2Body*>();
    _b2Bullets   = new std::vector<b2Body*>();

    /*asteroids to bodes*/
    for(int i = 0; i < 15; ++i) {
    	Asteroid* asteroid = Asteroid::create("asteroid.png", 3);
    	float randScale = randomBetween(0.3, 1.0);
    	asteroid->setScale(randScale);
    	asteroid->setVisible(false);
    	asteroid->setTag(1);
    	_batchNode->addChild(asteroid);
    	_asteroids->push_back(asteroid);
    	b2Body *temp = createb2CircleBody(asteroid, 0.0f, 0.0f, 0.0f, 680, 200, (55.0*randScale));
    	_b2Asteroids->push_back(temp);
    }
    /*lasers to bodes*/
    for(int i = 0; i < _ship->bullets->size(); ++i){
    	b2Body *temp = createb2CircleBody(_ship->bullets->at(i), 1.0f, 0.0f, 0.0f,
    	_ship->bullets->at(i)->getPosition().x/PTM_RATIO,
    	_ship->bullets->at(i)->getPosition().y/PTM_RATIO, 7.0f);
    	_b2Bullets->push_back(temp);
    }
/*debug GL*/
    if (DEBUG) {
    _debugDraw = new GLESDebugDraw(PTM_RATIO);
    _world->SetDebugDraw(_debugDraw);
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    _debugDraw->SetFlags(flags);
    }
/*end box*/
    this->showLives(_ship);
    this->showScore(_ship);

    this->setTouchEnabled(true);
    this->scheduleUpdate();
    this->setAccelerometerEnabled(true);

    return true;
}
void HelloWorld::setInvisible(CCNode* node){
	node->setVisible(false);
}
b2Body* HelloWorld::createb2CircleBody(CCNode* object, float density, float friction,
		                         float restitution, float x, float y, float radius){
	b2Body *temp;
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(x/PTM_RATIO, y/PTM_RATIO);
	ballBodyDef.userData = object;
	temp = _world->CreateBody(&ballBodyDef);

	/*b2PolygonShape spriteShape;
	spriteShape.SetAsBox(object->getContentSize().width/PTM_RATIO/2,
	    				 object->getContentSize().height/PTM_RATIO/2);*/
	b2CircleShape circle;
	circle.m_radius = radius / PTM_RATIO;

	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;//&spriteShape;
	ballShapeDef.density = density;
	ballShapeDef.friction = friction;
	ballShapeDef.restitution = restitution;
	temp->CreateFixture(&ballShapeDef);
	return temp;
}
void HelloWorld::setPositionb2Body(b2Body* body, float x, float y){
	body->SetTransform(b2Vec2(x/PTM_RATIO, y/PTM_RATIO), 0.0f);
}
void HelloWorld::draw(){
	if (DEBUG) {
		glDisable(GL_TEXTURE_2D);
		_world->DrawDebugData();
		glEnable(GL_TEXTURE_2D);
	}
}

void HelloWorld::didAccelerate(CCAcceleration* pAccelerationValue){
	CCSize winsize = CCDirector::sharedDirector()->getWinSize();
	#define KFILTERINGFACTOR 0.1
	#define KRESTACCELX -0.3
	#define KSHIPMAXPOINTSPERSEC (winsize.height*0.5)
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
void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event){
	_ship->shoot();
}
void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();
}

void HelloWorld::update(float dt) {
	CCSize winsize = CCDirector::sharedDirector()->getWinSize();
/*parallax */
	CCPoint backgroundScrollVert = ccp(-1000,0) ;
	_background->setPosition(ccpAdd(_background->getPosition(),ccpMult(backgroundScrollVert,dt)));

	CCArray *spaceDusts = CCArray::createWithCapacity(2);
	spaceDusts->addObject(_spacedust1) ;
	spaceDusts->addObject(_spacedust2) ;
	for (int ii = 0; ii < spaceDusts->count(); ii++ ) {
		CCSprite * spaceDust = (CCSprite *)(spaceDusts->objectAtIndex(ii)) ;
		float xPosition = _background->convertToWorldSpace(spaceDust->getPosition()).x;
		float size = spaceDust->getContentSize().width ;
		if (xPosition < -size) {
			_background->incrementOffset(ccp(spaceDust->getContentSize().width*2,0),spaceDust);
		}
	}
/*end parallax */
/*accelerometer*/

	float maxY = (winsize.height - _ship->getContentSize().height/2);
	float minY = _ship->getContentSize().height/2;
	float diff = _shipPositionY * dt;
	float newY = _ship->getPosition().y + diff;
	newY = MIN(MAX(newY, minY), maxY);
	_ship->setPosition(ccp(_ship->getPosition().x, newY));
/*end accelerometer*/

		/*first we move sprite's and then body's*/
/*sprite's animation*/
	float curTime = getTimeTick();
	float k = randomBetween(1.0, 3.2);
	if (curTime > _nextAsteroidSpawn){
		float randY = randomBetween(0, winsize.height); // ?
		float randms = randomBetween(0.2, 1.0)*1000;
		_nextAsteroidSpawn = randms + curTime;
		float duration = randomBetween(1.0, 7.0);
			Asteroid* asteroid = _asteroids->at(_nextAsteroid++);
		if(_nextAsteroid >= _asteroids->size())
			_nextAsteroid = 0;
			asteroid->stopAllActions();
			asteroid->setPosition(ccp(winsize.width + asteroid->getContentSize().width/2, randY));
			asteroid->setLives(3);
			asteroid->setVisible(true);
			asteroid->runAction(CCSequence::create(CCMoveBy::create(duration,ccp( -winsize.width - asteroid->getContentSize().width, 0)),
								 	 	    CCCallFuncN::create(this, callfuncN_selector(HelloWorld::setInvisible)),NULL));
	}
/*end sprite's animation*/
/*body's animation*/
	_world->Step(dt, 10, 10);
	for(b2Body *b = _world->GetBodyList(); b; b = b->GetNext()){
		if (b->GetUserData() != NULL){
			CCSprite* data = (CCSprite*) b->GetUserData();
			if (data->isVisible()) {
				b2Vec2 pos = b2Vec2(data->getPosition().x/PTM_RATIO,
									data->getPosition().y/PTM_RATIO);
				float angle = -1 * CC_DEGREES_TO_RADIANS(data->getRotation());
				b->SetTransform(pos, angle);
			}
		}
	}
/*end body's animation*/
/*body's collisions*/     // TODO: REFACTOR!!
	std::deque<CurrentContact>::iterator i;
	for(i = _collisionListener->contacts.begin(); i != _collisionListener->contacts.end(); ++i){
		CurrentContact contact = *i;
		b2Body *a = contact.a->GetBody();
		b2Body *b = contact.b->GetBody();
		if (a->GetUserData() != NULL && b->GetUserData() != NULL){
			CCSprite *sA = (CCSprite*)a->GetUserData();
			CCSprite *sB = (CCSprite*)b->GetUserData();

			if (sA->isVisible() && sB->isVisible()){
				if (sA->getTag() == 1) {// asteroid
					if (sB->getTag() == 2) {// laser
						Asteroid *ast = (Asteroid*)a->GetUserData();
						if (ast->getLives() <= 1){ // asteroid lives 0
							a->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
							b->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
							ast->setVisible(false);
							sB->setVisible(false);
							ast->stopAllActions();
							sB->stopAllActions();

							_ship->addScore(10);
							updateScore(_ship);
						} else {
							ast->subLife();
							b->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
							sB->setVisible(false); // hide laser
							sB->stopAllActions();
						}
					}
				}
				if (sA->getTag() == 2){  // laser
					if (sB->getTag() == 1){ // asteroid
						Asteroid *ast = (Asteroid*)b->GetUserData();
						if (ast->getLives() <= 1){ // asteroid lives 0
							a->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
							b->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
							ast->setVisible(false);
							sA->setVisible(false);
							ast->stopAllActions();
							sA->stopAllActions();

							_ship->addScore(10);
							updateScore(_ship);
						} else {
							ast->subLife();
							a->SetTransform(b2Vec2(-100.0, -100.0), 0.0f);
							sA->setVisible(false); // hide laser
							sA->stopAllActions();
						}
					}
				}
				if ((sA->getTag() == 0 && sB->getTag() == 1) ||
					(sA->getTag() == 1 && sB->getTag() == 0)) {
					if (_ship->getLives() > 0){
						_ship->subLife();
						updateLives(_ship);
						_ship->runAction(CCBlink::create(1.0, 4));
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
					}
				}
			}
		}
	}
/*end body's collisions*/
}
/* low + something; where something is value between 0 and high-low. (s.o.)*/
float HelloWorld::randomBetween(float low, float high){
	float random = ((float) rand() / (float) RAND_MAX);
	float diff = high - low;
	float r = random * diff;
	return low + r;
}
float HelloWorld::getTimeTick(){ // get time in milliseconds
	timeval time;
	gettimeofday(&time, NULL);
	unsigned long ms = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (float) ms;
}
void HelloWorld::showLives(Ship* ship) {
	stringstream ss;
	ss<<"Lives: ";
	ss<<ship->getLives();
	labelLives = CCLabelTTF::create(ss.str().c_str(), "Arail", 15);
	labelLives->setPosition(ccp(400, 300));
	this->addChild(labelLives);
}
void HelloWorld::showScore(Ship* ship) {
	stringstream ss;
	ss<<"Score: ";
	ss<<ship->getScore();
	labelScore = CCLabelTTF::create(ss.str().c_str(), "Arail", 15);
	labelScore->setPosition(ccp(400, 285));
	this->addChild(labelScore);
}
void HelloWorld::updateLives(Ship* ship) {
	stringstream ss;
	ss<<"Lives: ";
	ss<<ship->getLives();
	labelLives->setString(ss.str().c_str());
}
void HelloWorld::updateScore(Ship* ship) {
	stringstream ss;
	ss<<"Score: ";
	ss<<ship->getScore();
	labelScore->setString(ss.str().c_str());
}
void HelloWorld::pauseMenu(bool isLoose){

}
template<class T> string HelloWorld::toString(T t){
	stringstream ss;
	ss<<t;
	return ss.str();
}
