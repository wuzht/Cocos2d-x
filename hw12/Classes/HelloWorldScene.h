#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void menuItemWCallBack(Ref* pSender);

	void menuItemACallBack(Ref* pSender);

	void menuItemSCallBack(Ref* pSender);

	void menuItemDCallBack(Ref* pSender);

	void menuItemXCallBack(Ref* pSender);

	void menuItemYCallBack(Ref* pSender);

	void updateTime(float dt);

	void createMonster(float dt);

	void hitByMonster(float dt);

	void menuItemX();
        
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;

	bool isPlayerActing;
	void setPlayerSpriteFrame(int type);
	bool isPlayerAlive;
	int score;
	cocos2d::Label* scoreLabel;
	bool isPlayerFaceToEast;
};