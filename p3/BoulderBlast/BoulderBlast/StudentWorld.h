#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <list>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir), m_bonus(1000), m_jewels(0)
	{}

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	bool isSpaceEmpty(int x, int y);

	virtual int loadALevel();
	virtual int objectType(int type);
	virtual int numJewels() const { return m_jewels; }
	virtual int getBonus() const { return m_bonus; }

	virtual Actor* getPlayer() { return m_player; }
	virtual Actor* getObjectAt(int x, int y, list<Actor *> &actors);
	virtual Actor* getObjectWithout(int x, int y, Actor *p, list<Actor *> &orig, list<Actor *> &copy);
	virtual list<Actor *> actors() {
		list<Actor *> x = m_actors;
		return x; }

	virtual bool levelComplete();
	virtual bool bulletCollision(Actor *p, int x, int y, GraphObject::Direction dir);

	virtual void setExitOpen() { m_exitOpen = true; }
	virtual void decJewels() { m_jewels--; }
	virtual void checkDeadActors();
	virtual void setDisplayText();
	virtual void addActor(Actor *p);
	
private:
	Actor * m_player;
	list<Actor *> m_actors;
	virtual string levelName();

	int m_jewels;
	int m_bonus;
	int m_exitX, m_exitY;
	bool m_exitOpen;
};

#endif // STUDENTWORLD_H_
