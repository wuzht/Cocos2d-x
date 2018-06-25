#include "LoginRegisterScene.h"
#include "ui\CocosGUI.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "json\writer.h"
#include "json\stringbuffer.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;
using namespace cocos2d::ui;

cocos2d::Scene * LoginRegisterScene::createScene() {
	return LoginRegisterScene::create();
}

bool LoginRegisterScene::init() {
	if (!Scene::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto loginButton = MenuItemFont::create("Login", CC_CALLBACK_1(LoginRegisterScene::loginButtonCallback, this));
	if (loginButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + loginButton->getContentSize().height / 2;
		loginButton->setPosition(Vec2(x, y));
	}

	auto registerButton = MenuItemFont::create("Register", CC_CALLBACK_1(LoginRegisterScene::registerButtonCallback, this));
	if (registerButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + registerButton->getContentSize().height / 2 + 100;
		registerButton->setPosition(Vec2(x, y));
	}

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
		backButton->setPosition(Vec2(x, y));
	}

	auto menu = Menu::create(loginButton, registerButton, backButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	usernameInput = TextField::create("username", "arial", 24);
	if (usernameInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 100.0f;
		usernameInput->setPosition(Vec2(x, y));
		this->addChild(usernameInput, 1);
	}

	passwordInput = TextField::create("password", "arial", 24);
	if (passwordInput) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 130.0f;
		passwordInput->setPosition(Vec2(x, y));
		this->addChild(passwordInput, 1);
	}

	messageBox = Label::create("", "arial", 30);
	if (messageBox) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + visibleSize.height - 200.0f;
		messageBox->setPosition(Vec2(x, y));
		this->addChild(messageBox, 1);
	}

	return true;
}

void LoginRegisterScene::loginButtonCallback(cocos2d::Ref * pSender) {
	// Your code here
	HttpRequest* request = new HttpRequest();
	request->setUrl("localhost:8000/auth");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompleted, this));

	// rapidjson
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	document.AddMember("username", rapidjson::Value(usernameInput->getStringValue().c_str(), usernameInput->getStringValue().size()), allocator);
	document.AddMember("password", rapidjson::Value(passwordInput->getStringValue().c_str(), passwordInput->getStringValue().size()), allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	request->setRequestData(buffer.GetString(), strlen(buffer.GetString()));
	request->setTag("Login");
	// Cookies
	HttpClient::getInstance()->enableCookies(NULL);
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::registerButtonCallback(Ref * pSender) {
	// Your code here
	HttpRequest* request = new HttpRequest();
	request->setUrl("localhost:8000/users");
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(CC_CALLBACK_2(LoginRegisterScene::onHttpRequestCompleted, this));

	// write the post data
	std::string postData = "{\"username\": \"" + usernameInput->getStringValue()
						 + "\", \"password\": \"" + passwordInput->getStringValue() + "\"}";
	request->setRequestData(postData.c_str(), postData.size());
	request->setTag("Register");
	HttpClient::getInstance()->send(request);
	request->release();
}

void LoginRegisterScene::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response) {
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
		this->messageBox->setString(std::string(response->getHttpRequest()->getTag()) + " OK\n");	
	else
		this->messageBox->setString(std::string(response->getHttpRequest()->getTag()) + " Failed\n" + doc["msg"].GetString());
}

