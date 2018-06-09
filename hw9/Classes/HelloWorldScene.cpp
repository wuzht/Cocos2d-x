#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#define MUSIC_FILE_PATH "music.mp3"

USING_NS_CC;

bool isMusicPlaying = false;
bool isMusicPause = false;

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
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("bg.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

	auto myBtn = MenuItemImage::create(
		"btnNormal.png",
		"btnSeleted.png",
		CC_CALLBACK_1(HelloWorld::playMusicCallback, this)
	);
	myBtn->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 - 120));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	auto myMenu = Menu::create(myBtn, NULL);
	myMenu->setPosition(Vec2::ZERO);
	this->addChild(myMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
	CCDictionary* message = CCDictionary::createWithContentsOfFile("chinese.xml");
	const char* name = ((CCString*)message->valueForKey("name"))->getCString();
	CCLOG("my name: %s", name);
    auto label = Label::createWithTTF(name, "fonts/china.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/china.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

	auto label2 = Label::createWithTTF("16340246", "fonts/Milkshake.ttf", 24);
	if (label2 == nullptr)
	{
		problemLoading("'fonts/Milkshake.ttf'");
	}
	else
	{
		// position the label on the center of the screen
		label2->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - label2->getContentSize().height - 24));

		// add the label as a child to this layer
		this->addChild(label2, 1);
	}

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("sysu.png");
    if (sprite == nullptr)
    {
        problemLoading("'sysu.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

// click the button to play or resume music
void HelloWorld::playMusicCallback(cocos2d::Ref* pSender) {
	if (!isMusicPlaying) {
		isMusicPlaying = !isMusicPlaying;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(MUSIC_FILE_PATH, true);
	}
	else if (!isMusicPause)
	{
		isMusicPause = !isMusicPause;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}
	else {
		isMusicPause = !isMusicPause;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	}
	
}
