#include "GameScene.h"

USING_NS_CC;

Scene* GameSence::createScene()
{
	auto scene = Scene::create();
	auto layer = GameSence::create();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	stoneLayer = Layer::create();
	stoneLayer->setPosition(Vec2(0, 0));
	stoneLayer->setAnchorPoint(Vec2(0, 0));
	stoneLayer->addChild(stone);

	auto fixedStone = Sprite::create("stone.png");
	fixedStone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(fixedStone);

	this->addChild(stoneLayer, 1);

	// load the Sprite Sheet
	auto spriteCache = SpriteFrameCache::getInstance();
	spriteCache->addSpriteFramesWithFile("level-sheet.plist");

	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(Vec2(origin.x + visibleSize.width / 2, 0));

	mouseLayer = Layer::create();
	mouseLayer->setPosition(Vec2(0, origin.y + visibleSize.height / 2));
	mouseLayer->setAnchorPoint(Vec2(0, 0));
	mouseLayer->addChild(mouse);

	this->addChild(mouseLayer, 1);

	auto shootLabel = Label::createWithSystemFont("Shoot", "fonts/Marker Felt.ttf", 100);
	auto shootMenuItem = MenuItemLabel::create(shootLabel, CC_CALLBACK_1(GameSence::shootMenuCallback, this));
	shootMenuItem->setPosition(Vec2(visibleSize.width + origin.x - 200, visibleSize.height + origin.y - 100));

	auto menu = Menu::create(shootMenuItem, NULL);
	menu->setPosition(Vec2::ZERO);
	menu->setAnchorPoint(Vec2::ZERO);
	this->addChild(menu, 1);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {

	auto location = touch->getLocation();
	// add cheese
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese);
	// the mouse move to the cheese in 1 second
	auto moveTo = MoveTo::create(1, mouseLayer->convertToNodeSpace(location));
	mouse->runAction(moveTo);

	// the cheese is removed in 1 second (the mouse arrived)
	auto delayTime = DelayTime::create(1);
	auto func = CallFunc::create([this, cheese]() {
		this->removeChild(cheese);
	});
	auto seq = Sequence::create(delayTime, func, nullptr);
	this->runAction(seq);

	return true;
}

void GameSence::shootMenuCallback(Ref* pSender) {
	log("shoot!");
	
	auto location = mouse->getPosition();
	location = mouseLayer->convertToWorldSpace(location);
	// fire the stone
	auto moveTo = MoveTo::create(0.5, stoneLayer->convertToNodeSpace(location));
	stone->runAction(moveTo);

	auto delayTime = DelayTime::create(0.5);
	auto func = CallFunc::create([this, location]() {
		// mouse move away
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto newLocation = Point(CCRANDOM_0_1() * visibleSize.width, CCRANDOM_0_1() * visibleSize.height);
		auto moveTo = MoveTo::create(0.5, mouseLayer->convertToNodeSpace(newLocation));
		mouse->runAction(moveTo);
			
		// left a diamond
		auto diamond = Sprite::create("diamond.png");
		Animate* diamondAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("diamondAnimation"));
		diamond->runAction(RepeatForever::create(diamondAnimate));
		diamond->setPosition(this->convertToNodeSpace(location));
		this->addChild(diamond);

		auto delayTime2 = DelayTime::create(0.5);

		// remove the fired stone and put a stone back to the original place
		auto func2 = CallFunc::create([this]() {
			stoneLayer->removeChild(stone);
			stone = Sprite::create("stone.png");
			stone->setPosition(Vec2(560, 480));
			stoneLayer->addChild(stone);
		});

		auto seq2 = Sequence::create(delayTime2, func2, nullptr);
		this->runAction(seq2);
	});

	auto seq = Sequence::create(delayTime, func, nullptr);
	this->runAction(seq);
}
