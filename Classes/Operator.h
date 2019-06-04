
#include "cocos2d.h"

using namespace cocos2d;

class Operator : public cocos2d::Layer
{
public:
	virtual bool init();
	CREATE_FUNC(Operator);
	int AnyPress;
	Sprite* Player;
	Texture2D* picture;
	Animate *  Anime;
	void OnKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void OnKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void MoveUP(float dt);
	void MoveDOWN(float dt);
	void MoveLEFT(float dt);
	void MoveRIGHT(float dt);
	void start();
	void getPlayer();
	void getPicture(Texture2D* pic);
	void getAnime(Animate * Ani);
};

