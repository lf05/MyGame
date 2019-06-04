
#include "cocos2d.h"'

using namespace cocos2d;

class MainScene : public cocos2d::Scene
{
public:
	bool Attackable=1;
	bool TouchEnded = 1;
	Texture2D* LegsPic;
	Texture2D* BodyPic;
	static cocos2d::Scene* createScene();
	virtual bool init();
	Vector<SpriteFrame*> LegsFrames;
	Vector<SpriteFrame*> BodyFrames;
	Vector<SpriteFrame*> HandsFrames;
	Vector<SpriteFrame*> HeadFrames;
	void response(cocos2d::Ref * obj);
	Sprite* Legs;
	Sprite* Body;
	Sprite* Head;
	void menuCloseCallback(cocos2d::Ref* pSender);
	void Spawn(float dt);
	void Collide(float dt);
	void disappear(Node *who);
	bool onTouchBegan(Touch * touch, Event *event);
	void AttackSpeed(float dt);
	void Attack_Shoot(float dt);
	void Attack_Wave(float dt);
	// implement the "static create()" method manually
	CREATE_FUNC(MainScene);
	CCArray* projects;
	CCArray* targets;
	~MainScene();
	void onTouchMoved(Touch * ptouch, Event * pevent);
	void onTouchEnded(Touch * ptouch, Event * pevent);
	Vector<SpriteFrame*> Animation(const char * FrameName, int begin, int end);
	int AnyPress=0;
	void OnKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void OnKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void MoveUP(float dt);
	void MoveDOWN(float dt);
	void MoveLEFT(float dt);
	void MoveRIGHT(float dt);
	void AttackEnd(float dt);
	void AttackEndFlag(float dt);
	bool attack_end_flag=1;
	Point TouchPosition;
	bool MouseDown;
};


