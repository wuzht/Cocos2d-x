#include "ModifyUserScene.h"
#include "Utils.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "json\writer.h"
#include "json\stringbuffer.h"

using namespace cocos2d::network;

cocos2d::Scene * ModifyUserScene::createScene() {
	return ModifyUserScene::create();
}

bool ModifyUserScene::init() {
	if (!Scene::init()) return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto postDeckButton = MenuItemFont::create("Post Deck", CC_CALLBACK_1(ModifyUserScene::putDeckButtonCallback, this));
	if (postDeckButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + postDeckButton->getContentSize().height / 2;
		postDeckButton->setPosition(Vec2(x, y));
	}

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
		backButton->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(postDeckButton, backButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	deckInput = TextField::create("Deck json here", "arial", 24);
	if (deckInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 100.0f;
		deckInput->setPosition(Vec2(x, y));
		this->addChild(deckInput, 1);
	}

	messageBox = Label::create("", "arial", 30);
	if (messageBox) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height / 2;
		messageBox->setPosition(Vec2(x, y));
		this->addChild(messageBox, 1);
	}

	return true;
}

void ModifyUserScene::putDeckButtonCallback(Ref * pSender) {
	// Your code here
	HttpRequest* request = new HttpRequest();
	request->setUrl("localhost:8000/users");
	request->setRequestType(HttpRequest::Type::PUT);
	request->setResponseCallback(CC_CALLBACK_2(ModifyUserScene::onHttpRequestCompleted, this));

	// write the post data
	std::string postData = "{\"deck\":" + deckInput->getStringValue() + "}";

	request->setRequestData(postData.c_str(), postData.size());
	request->setTag("PUT");
	HttpClient::getInstance()->send(request);
	request->release();
}

void ModifyUserScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		this->messageBox->setString("PUT response failed\n");
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}
	std::vector<char> *buffer = response->getResponseData();
	log("Http Test, dump data: ");
	for (unsigned i = 0; i < buffer->size(); i++)
	{
		log("%c", (*buffer)[i]);
	}
	log("\n");

	rapidjson::Document doc;
	doc.Parse(buffer->data(), buffer->size());
	if (doc["status"] == true)
		this->messageBox->setString(std::string(response->getHttpRequest()->getTag()) + " OK\n");
	else
		this->messageBox->setString(std::string(response->getHttpRequest()->getTag()) + " Failed\n" + doc["msg"].GetString());
}
