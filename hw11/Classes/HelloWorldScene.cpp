#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
	return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	attackAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attackAnimation");

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	deadAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "deadAnimation");

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++)
	{
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.025f);
	runAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(runAnimation, "runAnimation");

	isPlayerActing = false;

	// W A S D X Y 键
	auto labelW = Label::createWithTTF("W", "fonts/arial.ttf", 36);
	auto labelA = Label::createWithTTF("A", "fonts/arial.ttf", 36);
	auto labelS = Label::createWithTTF("S", "fonts/arial.ttf", 36);
	auto labelD = Label::createWithTTF("D", "fonts/arial.ttf", 36);
	auto labelX = Label::createWithTTF("X", "fonts/arial.ttf", 36);
	auto labelY = Label::createWithTTF("Y", "fonts/arial.ttf", 36);

	auto menuItemW = MenuItemLabel::create(labelW, CC_CALLBACK_1(HelloWorld::menuItemWCallBack, this));
	auto menuItemA = MenuItemLabel::create(labelA, CC_CALLBACK_1(HelloWorld::menuItemACallBack, this));
	auto menuItemS = MenuItemLabel::create(labelS, CC_CALLBACK_1(HelloWorld::menuItemSCallBack, this));
	auto menuItemD = MenuItemLabel::create(labelD, CC_CALLBACK_1(HelloWorld::menuItemDCallBack, this));
	auto menuItemX = MenuItemLabel::create(labelX, CC_CALLBACK_1(HelloWorld::menuItemXCallBack, this));
	auto menuItemY = MenuItemLabel::create(labelY, CC_CALLBACK_1(HelloWorld::menuItemYCallBack, this));

	menuItemW->setPosition(Vec2(origin.x - 300, origin.y - 160));
	menuItemA->setPosition(Vec2(origin.x - 340, origin.y - 200));
	menuItemS->setPosition(Vec2(origin.x - 300, origin.y - 200));
	menuItemD->setPosition(Vec2(origin.x - 260, origin.y - 200));
	menuItemX->setPosition(Vec2(origin.x + 340, origin.y - 160));
	menuItemY->setPosition(Vec2(origin.x + 300, origin.y - 200));

	Vector<MenuItem*> menuItems;
	menuItems.pushBack(menuItemW);
	menuItems.pushBack(menuItemA);
	menuItems.pushBack(menuItemS);
	menuItems.pushBack(menuItemD);
	menuItems.pushBack(menuItemX);
	menuItems.pushBack(menuItemY);

	auto menu = Menu::createWithArray(menuItems);
	this->addChild(menu, 1);

	// 倒计时
	dtime = 180;
	time = Label::createWithTTF(std::to_string(dtime), "fonts/arial.ttf", 36);
	time->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 200));
	this->addChild(time);
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f);

	return true;
}

void HelloWorld::menuItemWCallBack(Ref* pSender) {
	setPlayerSpriteFrame();
	auto location = player->getPosition();
	if (location.y + 30 < visibleSize.height - 30) {
		player->runAction(MoveBy::create(0.2f, Vec2(0, 30)));
	}
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
}

void HelloWorld::menuItemACallBack(Ref* pSender) {
	setPlayerSpriteFrame();
	auto location = player->getPosition();
	if (location.x - 30 > 30) {
		player->runAction(MoveBy::create(0.2f, Vec2(-30, 0)));
	}
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
}

void HelloWorld::menuItemSCallBack(Ref* pSender) {
	setPlayerSpriteFrame();
	auto location = player->getPosition();
	if (location.y - 30 > 30) {
		player->runAction(MoveBy::create(0.2f, Vec2(0, -30)));
	}
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
}

void HelloWorld::menuItemDCallBack(Ref* pSender) {
	setPlayerSpriteFrame();
	auto location = player->getPosition();
	if (location.x + 30 < visibleSize.width - 30) {
		player->runAction(MoveBy::create(0.2f, Vec2(30, 0)));
	}
	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation")));
}

void HelloWorld::menuItemXCallBack(Ref* pSender) {
	if (isPlayerActing)
		return;
	isPlayerActing = true;
	auto callFunc = CallFunc::create([&] {
		isPlayerActing = false;
	});
	auto seq = Sequence::createWithTwoActions(
		Animate::create(AnimationCache::getInstance()->getAnimation("deadAnimation")),
		callFunc
	);
	player->runAction(seq);

	// 血条
	if (pT->getPercentage() - 20 >= 0) {
		pT->setPercentage(pT->getPercentage() - 20);
	}
	else
	{
		pT->setPercentage(0);
	}
}

void HelloWorld::menuItemYCallBack(Ref* pSender) {
	if (isPlayerActing)
		return;
	isPlayerActing = true;
	auto callFunc = CallFunc::create([&] {
		isPlayerActing = false;
	});
	auto seq = Sequence::createWithTwoActions(
		Animate::create(AnimationCache::getInstance()->getAnimation("attackAnimation")),
		callFunc
	);
	player->runAction(seq);

	// 血条
	if (pT->getPercentage() + 20 <= 100) {
		pT->setPercentage(pT->getPercentage() + 20);
	}
	else
	{
		pT->setPercentage(100);
	}
}

void HelloWorld::setPlayerSpriteFrame() {
	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame3 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(68 * 3, 0, 68, 101)));
	//使player使用第三帧
	player->setSpriteFrame(frame3);
}

void HelloWorld::updateTime(float dt) {
	dtime--;
	if (dtime < 0) {
		unschedule(schedule_selector(HelloWorld::updateTime));
		return;
	}
	time->setString(std::to_string(dtime));
}
