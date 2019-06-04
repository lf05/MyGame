
#include "MainScene.h"
#include "SimpleAudioEngine.h"

#define UP 28
#define LEFT 26
#define RIGHT 27
#define DOWN 29
USING_NS_CC;

Scene* MainScene::createScene()
{
	return MainScene::create();
}

SEL_SCHEDULE move[4] = {
	schedule_selector(MainScene::MoveLEFT),
	schedule_selector(MainScene::MoveRIGHT),
	schedule_selector(MainScene::MoveUP),
	schedule_selector(MainScene::MoveDOWN) };

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool MainScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(MainScene::menuCloseCallback, this));

	if (closeItem == nullptr ||
		closeItem->getContentSize().width <= 0 ||
		closeItem->getContentSize().height <= 0)
	{
		problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
		float y = origin.y + closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}


	auto label = Label::createWithTTF("Solar", "fonts/Marker Felt.ttf", 24);
	if (label == nullptr)
	{
		problemLoading("'fonts/Marker Felt.ttf'");
	}
	else
	{
		label->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height - label->getContentSize().height));

		this->addChild(label, 1);
	}

	auto sprite = Sprite::create("Planet.Texture.Mip.0.png");
	if (sprite == nullptr)
	{
		problemLoading("'Planet.Texture.Mip.0.png'");
	}
	else
	{
	
		sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

		this->addChild(sprite, 0);
	}

	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);

	projects = CCArray::create();
	targets = CCArray::create();
	projects->retain();
	targets->retain();

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Player.plist");
	HandsFrames = Animation("Armor_Body_177-%d.png", 1, 3);

	BodyFrames = Animation("Armor_Body_177-%d.png", 6, 19);
	LegsFrames= Animation("Armor_Legs_112-%d.png", 6, 19);
	HeadFrames=Animation("Armor_Head_171-%d.png", 6, 19);

	Legs = Sprite::createWithSpriteFrameName("Armor_Legs_112-0.png");
	Body = Sprite::createWithSpriteFrameName("Armor_Body_177-0.png");
	Head = Sprite::createWithSpriteFrameName("Armor_Head_171-0.png");

	Legs->setPosition(20, visibleSize.height / 2);
	this->addChild(Legs);

	Head->setPosition(20, visibleSize.height / 2 + 15);
	this->addChild(Head);

	Body->setPosition(20, visibleSize.height / 2+15);
	this->addChild(Body);


	auto listener2 = EventListenerKeyboard::create();
	listener2->onKeyPressed = CC_CALLBACK_2(MainScene::OnKeyPressed, this);
	listener2->onKeyReleased = CC_CALLBACK_2(MainScene::OnKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);

	auto listener1 = EventListenerTouchOneByOne::create();
	listener1->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	listener1->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	listener1->onTouchEnded= CC_CALLBACK_2(MainScene::onTouchEnded, this);
	listener1->setSwallowTouches(true);
   	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener1,this);
	
	this->schedule(schedule_selector(MainScene::Spawn), 2);
	this->schedule(schedule_selector(MainScene::Collide), 1 / 60);

	this->schedule(schedule_selector(MainScene::AttackSpeed), 1);
	this->schedule(schedule_selector(MainScene::Attack_Wave), 1);
	//this->schedule(schedule_selector(MainScene::Attack_Shoot), 1);

	return true;
}

Vector<SpriteFrame*> MainScene::Animation(const char* FrameName, int begin, int end)
{
	auto spritecache = SpriteFrameCache::getInstance();
	char str[100];
	Vector<SpriteFrame*> animFrames;
	for (int i = begin; i <= end; i++)
	{
		sprintf(str, FrameName, i);
		animFrames.pushBack(spritecache->getSpriteFrameByName(str));
	}
	return animFrames;
}


MainScene::~MainScene()
{
	if (projects != NULL)
		projects->release();
	if (targets != NULL)
		targets->release();
}
void MainScene::AttackSpeed(float dt)
{
	Attackable = 1;
}

void MainScene::Attack_Shoot(float dt)
{
	if (Attackable&&MouseDown)
	{
		Attackable = 0;
		attack_end_flag = 0;

		this->unschedule(schedule_selector(MainScene::AttackSpeed));
		this->schedule(schedule_selector(MainScene::AttackSpeed), 1);

		this->unschedule(schedule_selector(MainScene::Attack_Shoot));
		this->schedule(schedule_selector(MainScene::Attack_Shoot), 1);

		this->scheduleOnce(schedule_selector(MainScene::AttackEndFlag), 60 / 60);

		auto visibleSize = Director::getInstance()->getVisibleSize();
		auto PlayerPosition = Body->getPosition();

		if (TouchPosition.x < PlayerPosition.x)
		{
			Legs->setFlipX(true);
			Body->setFlipX(true);
			Head->setFlipX(true);
		}
		else
		{
			Legs->setFlipX(false);
			Body->setFlipX(false);
			Head->setFlipX(false);
		}

		auto bullet = Sprite::create("Bullet.png");
		bullet->setPosition(PlayerPosition.x, PlayerPosition.y);
		this->addChild(bullet);
		projects->addObject(bullet);

		bullet->setTag(2);

		double range = 300;
		double l = sqrt((TouchPosition.x - PlayerPosition.x)*(TouchPosition.x - PlayerPosition.x) + (TouchPosition.y - PlayerPosition.y)*(TouchPosition.y - PlayerPosition.y));
		double x = range * (TouchPosition.x - PlayerPosition.x) / l + PlayerPosition.x;
		double y = range * (TouchPosition.y - PlayerPosition.y) / l + PlayerPosition.y;

		auto move = MoveTo::create(3, Vec2(x, y));
		auto disappear = CallFuncN::create(this, callfuncN_selector(MainScene::disappear));
		auto attackmove = Sequence::create(move, disappear, NULL);

		bullet->runAction(attackmove);

		float dir1 = TouchPosition.x - PlayerPosition.x - TouchPosition.y + PlayerPosition.y;
		float dir2 = TouchPosition.x - PlayerPosition.x +TouchPosition.y - PlayerPosition.y;

		Body->cleanup();
		if (dir1*dir2 >= 0)
		{
			Body->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Armor_Body_177-3.png"));
		}
		else if (dir1 < 0 && dir2>0)
		{
			Body->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Armor_Body_177-2.png"));
		}
		else if (dir1 > 0 && dir2 < 0)
		{
			Body->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("Armor_Body_177-4.png"));
		}

	}
	return;
}

void MainScene::Attack_Wave(float dt)
{
		if (Attackable&&MouseDown)
		{
			Attackable = 0;
			attack_end_flag = 0;

			this->unschedule(schedule_selector(MainScene::AttackSpeed));
			this->schedule(schedule_selector(MainScene::AttackSpeed), 1);

			this->unschedule(schedule_selector(MainScene::Attack_Wave));
			this->schedule(schedule_selector(MainScene::Attack_Wave), 1);

			this->scheduleOnce(schedule_selector(MainScene::AttackEndFlag),50.0f/60);

			auto visibleSize = Director::getInstance()->getVisibleSize();
			auto PlayerPosition = Body->getPosition();

			if (TouchPosition.x < PlayerPosition.x)
			{
				Legs->setFlipX(true);
				Body->setFlipX(true);
				Head->setFlipX(true);
			}
			else
			{
				Legs->setFlipX(false);
				Body->setFlipX(false);
				Head->setFlipX(false);
			}
			
			auto bullet = Sprite::create("Bullet.png");
			bullet->setPosition(PlayerPosition.x, PlayerPosition.y);
			this->addChild(bullet);
			projects->addObject(bullet);

			bullet->setTag(2);

			double range = 300;
			double l = sqrt((TouchPosition.x - PlayerPosition.x)*(TouchPosition.x - PlayerPosition.x) + (TouchPosition.y - PlayerPosition.y)*(TouchPosition.y - PlayerPosition.y));
			double x = range * (TouchPosition.x - PlayerPosition.x) / l + PlayerPosition.x;
			double y = range * (TouchPosition.y - PlayerPosition.y) / l + PlayerPosition.y;

			auto move = MoveTo::create(3, Vec2(x, y));
			auto disappear = CallFuncN::create(this, callfuncN_selector(MainScene::disappear));
			auto attackmove = Sequence::create(move, disappear, NULL);

			bullet->runAction(attackmove);

			Head->stopActionByTag(10);

			auto Headsp = Sprite::createWithSpriteFrameName("Armor_Head_171-0.png");
			Head->setDisplayFrame(Headsp->getDisplayFrame());

			auto animation = Animation::createWithSpriteFrames(HandsFrames, 20.0f/60, 1);
			auto animate = Animate::create(animation);
			animate->setTag(12);
			Body->stopActionByTag(11);
			Body->stopActionByTag(12);
			Body->runAction(animate);
		}
	return;
}
bool MainScene::onTouchBegan(Touch * ptouch, Event *pevent)
{
	TouchPosition = ptouch->getLocation();
	MouseDown = 1;
	if (Attackable)
	{
		this->Attack_Wave(0);
	}
	return true;
}
void MainScene::onTouchMoved(Touch * ptouch, Event *pevent)
{
	TouchPosition = ptouch->getLocation();
	return;
}
void MainScene::onTouchEnded(Touch * ptouch, Event *pevent)
{
	MouseDown = 0;
	this->schedule(schedule_selector(MainScene::AttackEnd), 1 / 60);
}
void MainScene::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();
}

void MainScene::Collide(float dt)
{
	auto proj = Array::create();
	proj->retain();
	auto targ = Array::create();
	targ->retain();
	Object*iproj;
	Object*itarg;

	CCARRAY_FOREACH(targets, itarg)
	{
		Sprite* target = (Sprite*)itarg;
		auto targetZone = CCRectMake(target->getPositionX(), target->getPositionY(), target->getContentSize().width, target->getContentSize().height);
		CCARRAY_FOREACH(projects, iproj)
		{
			Sprite* project = (Sprite*)iproj;
			auto projectZone = CCRectMake(project->getPositionX(), project->getPositionY(), project->getContentSize().width, project->getContentSize().height);

			if (projectZone.intersectsRect(targetZone))
			{
				proj->addObject(iproj);
				targ->addObject(itarg);
			}
		}
	}
	CCARRAY_FOREACH(proj, iproj)
	{
		projects->removeObject(iproj);
		Sprite* project = (Sprite*)iproj;
		project->removeFromParentAndCleanup(TRUE);
	}
	CCARRAY_FOREACH(targ, itarg)
	{
		targets->removeObject(itarg);
		Sprite* target = (Sprite*)itarg;
		target->removeFromParentAndCleanup(TRUE);
	}
}

void MainScene::Spawn(float dt)
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto tree = Sprite::create("monster.png");
	int y = rand() % (int)(visibleSize.height);
	tree->setPosition(visibleSize.width, y);
	this->addChild(tree);
	targets->addObject(tree);
	tree->setTag(1);

	auto move = MoveTo::create(1.5, Vec2(0, y));
	auto disappear = CallFuncN::create(this, callfuncN_selector(MainScene::disappear));
	auto seq = Sequence::create(move, disappear, NULL);
	tree->runAction(seq);
}
void MainScene::disappear(Node * who)
{
	who->removeFromParentAndCleanup(TRUE);
	int tag = who->getTag();
	if (1 == tag)
		targets->removeObject(who);
	else if (2 == tag)
		projects->removeObject(who);
}

void MainScene::OnKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
	int keycode = (int)keyCode - 26;
	if (0 <= keycode && 3 >= keycode)
	{
		AnyPress++;
		if (AnyPress)
		{

			auto animation = Animation::createWithSpriteFrames(LegsFrames, 1.0f / 8);
			auto Animate = RepeatForever::create(Animate::create(animation));
			Animate->setTag(13);
			Legs->stopActionByTag(13);
			Legs->runAction(Animate);

			if (attack_end_flag && !MouseDown)
			{
				auto animation1 = Animation::createWithSpriteFrames(HeadFrames, 1.0f / 8);

				auto animate1 = RepeatForever::create(Animate::create(animation1));
				animate1->setTag(10);
				Head->stopActionByTag(10);
				Head->runAction(animate1);

				auto animation2 = Animation::createWithSpriteFrames(BodyFrames, 1.0f / 8);
				auto animate2 = RepeatForever::create(Animate::create(animation2));
				animate2->setTag(11);
				Body->stopActionByTag(12);
				Body->stopActionByTag(11);
				Body->runAction(animate2);
			}
		}
		this->schedule(move[keycode], 1 / 60);
	}
}

void MainScene::OnKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	int keycode = (int)keyCode - 26;
	if (0 <= keycode && 3 >= keycode)
	{
		AnyPress--;
		this->unschedule(move[keycode]);
		if (!AnyPress)
		{
			Legs->stopActionByTag(13);
			auto Legssp = Sprite::createWithSpriteFrameName("Armor_Legs_112-0.png");
			Legs->setDisplayFrame(Legssp->getDisplayFrame());

			
			if (attack_end_flag)
			{
				Head->stopActionByTag(10);
				auto Headsp = Sprite::createWithSpriteFrameName("Armor_Head_171-0.png");
				Head->setDisplayFrame(Headsp->getDisplayFrame());

				Body->stopActionByTag(11);
				Body->stopActionByTag(12);
				auto Bodysp = Sprite::createWithSpriteFrameName("Armor_Body_177-0.png");
				Body->setDisplayFrame(Bodysp->getDisplayFrame());
			}

		}
	}
}


void MainScene::MoveUP(float dt)
{
	MoveBy* move = MoveBy::create(1 / 60, Vec2(0, 1));
	Legs->runAction(move);
	Body->runAction(move->clone());
	Head->runAction(move->clone());
}
void MainScene::MoveDOWN(float dt)
{

	MoveBy* move = MoveBy::create(1 / 60, Vec2(0, -1));
	Legs->runAction(move);
	Body->runAction(move->clone());
	Head->runAction(move->clone());
}
void MainScene::MoveLEFT(float dt)
{
	if (attack_end_flag && !MouseDown)
	{
		Legs->setFlippedX(true);
		Body->setFlippedX(true);
		Head->setFlippedX(true);
	}
	MoveBy* move = MoveBy::create(1 / 60, Vec2(-1, 0));
	Legs->runAction(move);
    Body->runAction(move->clone());
	Head->runAction(move->clone());
}
void MainScene::MoveRIGHT(float dt)
{
	if (attack_end_flag&&!MouseDown)
	{
		Legs->setFlippedX(false);
		Body->setFlippedX(false);
		Head->setFlippedX(false);
	}
	MoveBy* move = MoveBy::create(1.0f/ 60, Vec2(1, 0));
	Legs->runAction(move);
	Body->runAction(move->clone());
	Head->runAction(move->clone());
}
void MainScene::AttackEnd(float dt)
{
	if (attack_end_flag&&!MouseDown)
	{
		Body->stopActionByTag(12);
		Body->stopActionByTag(11);
		if (AnyPress)
		{
			auto animation1 = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(HeadFrames, 1.0f / 8)));
			animation1->setTag(10);
			Head->runAction(animation1);

			auto animation2 = RepeatForever::create(Animate::create(Animation::createWithSpriteFrames(BodyFrames, 1.0f / 8)));
			animation2->setTag(11);
			Body->runAction(animation2);
		}
		else
		{
			auto Bodysp = Sprite::createWithSpriteFrameName("Armor_Body_177-0.png");
			Body->setDisplayFrame(Bodysp->getDisplayFrame());

			auto Headsp = Sprite::createWithSpriteFrameName("Armor_Head_171-0.png");
			Head->setDisplayFrame(Headsp->getDisplayFrame());
		}
		this->unschedule(schedule_selector(MainScene::AttackEnd));
	}
}
void MainScene::AttackEndFlag(float dt)
{
	attack_end_flag = 1;
}
