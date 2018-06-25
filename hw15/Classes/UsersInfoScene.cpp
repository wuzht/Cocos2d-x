#include "UsersInfoScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

using namespace cocos2d::network;
using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
	return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
	if (!Scene::init()) return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
	if (getUserButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + getUserButton->getContentSize().height / 2;
		getUserButton->setPosition(Vec2(x, y));
	}

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
		backButton->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(getUserButton, backButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	limitInput = TextField::create("limit", "arial", 24);
	if (limitInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 100.0f;
		limitInput->setPosition(Vec2(x, y));
		this->addChild(limitInput, 1);
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

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
	// Your code here
	HttpRequest* request = new HttpRequest();
	request->setUrl("localhost:8000/users?limit=" + limitInput->getString());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpRequestCompleted, this));
	HttpClient::getInstance()->send(request);
	request->release();
}

void UsersInfoScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
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
	{
		std::string boxMsg;
		for (int i = 0; i < doc["data"].Size(); i++)
		{
			boxMsg += "Username: ";
			boxMsg += (doc["data"][i]["username"]).GetString();
			boxMsg += "\nDeck:\n";
			// 输出deck
			for (int j = 0; j < doc["data"][i]["deck"].Size(); j++)
			{
				for (auto& it : doc["data"][i]["deck"][j].GetObjectW()) {
					boxMsg += "  ";
					boxMsg += it.name.GetString();
					boxMsg += ": ";
					// 直接+=it.value.GetInt()会出现乱码，要先转化成字符串再拼接
					int num = it.value.GetInt();
					char temp[20];
					sprintf(temp, "%d", num);
					log("%s", temp);
					boxMsg += std::string(temp);
					boxMsg += "\n";
				}
				boxMsg += "  ---\n";
			}
			boxMsg += "---\n";
		}
		this->messageBox->setString(boxMsg);
	}
	else
	{
		this->messageBox->setString(doc["msg"].GetString());
	}
}
