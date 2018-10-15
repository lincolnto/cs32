#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <cstdlib>
#include <iostream>

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, 
		Direction dir, StudentWorld *p, bool state,
		int hp = 0, int ammo = 0 )
		: GraphObject(imageID, startX, startY, dir), m_hp(hp), m_ammo(ammo)
	{
		m_world = p;
		initMembers(state, imageID);
	};
	void setAlive(bool x)	{  m_alive = x; }
	bool isAlive() const {  return m_alive;  }

	StudentWorld* getWorld() const { return m_world; }

	int getItemType() const { return m_itemType; }
	virtual int getHP() const { return m_hp; }
	virtual void decHP() { m_hp -= 2; }
	virtual void incHP(int x) { m_hp += x; }
	virtual int getAmmo() const { return m_ammo; }
	virtual void incAmmo(int x) { m_ammo += x; }
	virtual void decAmmo() { m_ammo--; }
	virtual int characterMove(int destX, int destY) { return -2; }
	
	virtual void collectItem(int type, Actor *item) { return; }
	virtual void damage(int type) { return; }
	virtual void doSomething()	{ return; }
	virtual void moveInDir(Direction dir);
	virtual void initMembers(bool state, int imageID); // Sets visibility and alive state
	
private:
	bool m_alive;
	int m_itemType;
	int m_hp, m_ammo;
	StudentWorld* m_world;
	/*
	int			m_imageID; --
	bool		m_visible;
	double		m_x; --
	double		m_y; --
	double		m_destX; --
	double		m_destY; --
	double		m_brightness; --
	int			m_animationNumber; --
	Direction	m_direction; --
	*/
};

//////////////////////////////////////////////////////
//					Objects							//
//////////////////////////////////////////////////////

class Wall : public Actor
{
public:
	Wall(int startX, int startY, StudentWorld *p) 
		: Actor(IID_WALL, startX, startY, none, p, true)
	{};
private:
};

class Hole : public Actor
{
public:
	Hole(int startX, int startY, StudentWorld *p)
		: Actor(IID_HOLE, startX, startY, none, p, true)
	{}
	
};

class Exit : public Actor
{
public:
	Exit(int startX, int startY, StudentWorld *p)
		: Actor(IID_EXIT, startX, startY, none, p, false)
	{}
	virtual void doSomething();
};

class Bullet : public Actor
{
public:
	Bullet(int startX, int startY, StudentWorld *p, Direction dir)
		: Actor(IID_BULLET, startX, startY, dir, p, true)
	{}
	virtual void doSomething();
private:
};

class KBotFactory : public Actor
{
public:
	KBotFactory(int startX, int startY, StudentWorld *p, int botType)
		: Actor(IID_ROBOT_FACTORY, startX, startY, none, p, true),
		spawnType(botType)
	{
		if (getX() + 3 < VIEW_HEIGHT)
			topBound = getX() + 3;
		else topBound = VIEW_HEIGHT;
		if (getX() - 3 > 0)
			botBound = getX() - 3;
		else botBound = 0;
		if (getY() + 3 < VIEW_WIDTH)
			rightBound = getY() + 3;
		else rightBound = VIEW_WIDTH;
		if (getY() - 3 > 0)
			leftBound = getY() - 3;
		else leftBound = 0;
	}
	void doSomething();
private:
	int botsInRegion;
	int topBound, botBound, leftBound, rightBound;
	bool botOnFactory;
	int spawnType;
};

//////////////////////////////////////////////////////
//				Movable Characters					//
//////////////////////////////////////////////////////

class Character : public Actor // Players/robots
{
public:
	Character(int imageID, int startX, int startY, int hp, unsigned int ammo, 
		StudentWorld *p, Direction dir)
		: Actor(imageID, startX, startY, dir, p, true, hp, ammo)
	{};
	virtual int characterMove(int destX, int destY);
	virtual void collectItem(int type, Actor *item);
	virtual void damage(int type);
	virtual void fireBullet();
	
private:
};
class Boulder : public Character
{
public:
	Boulder(int startX, int startY, StudentWorld *p)
		: Character(IID_BOULDER, startX, startY, 10, 0, p, none)
	{}
};
class Player : public Character
{
public:
	Player(int startX, int startY, StudentWorld *p) 
		:  Character(IID_PLAYER, startX, startY, 20, 20, p, right)
	{};
	virtual void doSomething();
private:
};
class Robot : public Character
{
public:
	Robot(int startX, int startY, int imageID, int hp, StudentWorld *p, Direction dir)
		: Character(imageID, startX, startY, hp, 99999, p, dir)
	{
		ticks = curTicks = -1;
	};	
	virtual bool canFireAtPlayer();
	virtual void initTicks(int x)	{	ticks = curTicks = x;	}
	virtual int getTicks() const { return curTicks; }
	virtual void decTicks() 
	{
		if (curTicks == 0) curTicks = ticks;
		else curTicks--;
	}
private:
	int ticks;
	int curTicks;
};
class Snarlbot : public Robot
{
public:
	Snarlbot(int startX, int startY, StudentWorld *p, Direction dir)
		: Robot(startX, startY, IID_SNARLBOT, 10, p, dir)
	{};
	virtual void doSomething();

private:
};
class HorizSnarlbot : public Snarlbot
{
public:
	HorizSnarlbot(int startX, int startY, StudentWorld *p)
		: Snarlbot(startX, startY, p, right)
	{};
private:

};
class VertSnarlbot : public Snarlbot
{
public:
	VertSnarlbot(int startX, int startY, StudentWorld *p)
		: Snarlbot(startX, startY, p, down)
	{};
private:

};
class Kleptobot : public Robot
{
public:
	Kleptobot(int startX, int startY, StudentWorld *p, int imageID = IID_KLEPTOBOT, int hp = 5)
		: Robot(startX, startY, imageID, hp, p, right)
	{
		m_distanceBeforeTurning = rand() % 7 + 1;
		hasItem = false;
	}
	virtual void doSomething();
	virtual void damage(int type);
	virtual void moveInRandDir();
private:
	int m_distanceBeforeTurning;
	bool hasItem;
	Actor *item;

};
class AngryKleptobot : public Kleptobot
{
public:
	AngryKleptobot(int startX, int startY, StudentWorld *p)
		: Kleptobot(startX, startY, p, IID_ANGRY_KLEPTOBOT, 8)
	{};
};
//////////////////////////////////////////////////////
//						Goodies						//
//////////////////////////////////////////////////////

class Goodie : public Actor
{
public:
	Goodie(int startX, int startY, int imageID, StudentWorld *p)
		: Actor(imageID, startX, startY, none, p, true)
	{}	
private:
};
class Jewel : public Goodie
{
public:
	Jewel(int startX, int startY, StudentWorld *p)
		: Goodie(startX, startY, IID_JEWEL, p)
	{}
private:
};
class RestoreHP : public Goodie
{
public:
	RestoreHP(int startX, int startY, StudentWorld *p)
		: Goodie(startX, startY, IID_RESTORE_HEALTH, p)
	{}
private:
};
class ExtraLife : public Goodie
{
public:
	ExtraLife(int startX, int startY, StudentWorld *p)
		: Goodie(startX, startY, IID_EXTRA_LIFE, p)
	{}
private:
};
class Ammo : public Goodie
{
public:
	Ammo(int startX, int startY, StudentWorld *p)
		: Goodie(startX, startY, IID_AMMO, p)
	{}
private:
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
