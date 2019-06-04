#include "Operator.h"
#include"MainScene.h"

bool Operator::init()
{
	return true;
}
SEL_SCHEDULE move[4] = {
	schedule_selector(Operator::MoveLEFT),
	schedule_selector(Operator::MoveRIGHT),
	schedule_selector(Operator::MoveUP),
	schedule_selector(Operator::MoveDOWN) };

void Operator::start()
{
	auto listener2 = EventListenerKeyboard::create();
	listener2->onKeyPressed = CC_CALLBACK_2(Operator::OnKeyPressed,this);
	listener2->onKeyReleased = CC_CALLBACK_2(Operator::OnKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);
}
void Operator::getPlayer()
{
	log("Player");
	auto MainScene = MainScene::create();
	MainScene->Player->removeAllChildrenWithCleanup(TRUE);
}
void Operator::getPicture(Texture2D* pic)
{
	picture = pic;
}
void Operator::getAnime(Animate* Ani)
{
	Anime = Ani;
}
void Operator::OnKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
	int keycode = (int)keyCode - 26;
	if (0 <= keycode && 3 >= keycode)
	{
		if (!AnyPress)
		{
			Player->cleanup();
			Player->runAction(RepeatForever::create(Anime));
		}
		AnyPress++;
		if (keycode == 0)Player->setFlippedX(false);
		if (keycode == 1)Player->setFlippedX(true);
		this->schedule(move[keycode], 1 / 60);
	}
}

void Operator::OnKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	int keycode = (int)keyCode - 26;
	if (0 <= keycode && 3 >= keycode)
	{
		AnyPress--;
		this->unschedule(move[keycode]);
		if (!AnyPress)
		{
			Player->cleanup();
			auto sp = Sprite::createWithTexture(picture, Rect(8, 8, 26, 46));
			Player->setDisplayFrame(sp->getDisplayFrame());
		}
	}
}


void Operator::MoveUP(float dt)
{
	MoveBy* move = MoveBy::create(1 / 60, Vec2(0, 1));
	Player->runAction(move);
}
void Operator::MoveDOWN(float dt)
{

	MoveBy* move = MoveBy::create(1 / 60, Vec2(0, -1));
	Player->runAction(move);
}
void Operator::MoveLEFT(float dt)
{
	MoveBy* move = MoveBy::create(1 / 60, Vec2(-1, 0));
	Player->runAction(move);
}
void Operator::MoveRIGHT(float dt)
{
	MoveBy* move = MoveBy::create(1 / 60, Vec2(1, 0));
	Player->runAction(move);
}
