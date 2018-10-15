#include "StudentWorld.h"
#include <string>
using namespace std;

bool spotPassable(int destX, int destY, Actor *p, list<Actor *> &orig);

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Main functions
int StudentWorld::init()
{
	int x = loadALevel();
	if (x == -2) // Level bad format
		return GWSTATUS_LEVEL_ERROR;
	else if (x == -1)
		return GWSTATUS_PLAYER_WON;
	return GWSTATUS_CONTINUE_GAME;
}
int StudentWorld::move()
{
	setDisplayText();	// Update Game Status line
	if (m_player->isAlive())	// Ask all actors to do something
		m_player->doSomething();
	else return GWSTATUS_PLAYER_DIED;
	for (list<Actor *>::const_iterator i = m_actors.begin(); i != m_actors.end(); i++)
	{
		Actor *p = *i;
		if (p->isAlive())
		{
			p->doSomething();			// Ask all actors to do something
			// checkDeadActors();			// Check for dead actors & remove
			if (!m_player->isAlive())			// Check if player is still alive
			{
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
		}
	}
	checkDeadActors();		// Delete any dead actors
	if (m_bonus > 0)
		m_bonus--;	// Reduce bonus points
	
	// Check if player completed level
	if (levelComplete())
		return GWSTATUS_FINISHED_LEVEL;
	return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::cleanUp()
{
	for (list<Actor *>::const_iterator i = m_actors.begin(); i != m_actors.end();)
	{
		Actor *p = *i;
		i = m_actors.erase(i);
		delete p;
	}
	delete m_player;
	return;
}

// Helper functions
int StudentWorld::loadALevel()
{
	string	curLevel = levelName();
	Level	lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(curLevel);
	if (result == Level::load_fail_file_not_found || getLevel() > 99)
		return -1;
	if (result == Level::load_fail_bad_format)
		return -2;		//	something	bad	happened!
	// otherwise the load was successful and you can access the
	// contents of the level – here’s an example

	// Allocate all actors
	for (int x = 0; x <= VIEW_WIDTH - 1; x++)
	{
		for (int y = 0; y <= VIEW_HEIGHT - 1; y++)
		{
			int startX = x, startY = y;
			Level::MazeEntry item = lev.getContentsOf(x, y);
			switch (item)
			{
			case Level::empty:
				break;
			case Level::wall:
				m_actors.push_back(new Wall(startX, startY, this));
				break;
			case Level::player:
				m_player = new Player(startX, startY, this);
				break;
			case Level::boulder:
				m_actors.push_back(new Boulder(startX, startY, this));
				break;
			case Level::hole:
				m_actors.push_back(new Hole(startX, startY, this));
				break;
			case Level::exit:
				m_actors.push_back(new Exit(startX, startY, this));
				m_exitX = startX;	m_exitY = startY;
				m_exitOpen = false;
				break;
			case Level::horiz_snarlbot:
				m_actors.push_back(new HorizSnarlbot(startX, startY, this));
				break;
			case Level::vert_snarlbot:
				m_actors.push_back(new VertSnarlbot(startX, startY, this));
				break;
			case Level::kleptobot_factory:
				m_actors.push_back(new KBotFactory(startX, startY, this, 0));
				break;
			case Level::angry_kleptobot_factory:
				m_actors.push_back(new KBotFactory(startX, startY, this, 1));
				break;
			case Level::jewel:
				m_jewels++;
				m_actors.push_back(new Jewel(startX, startY, this));
				break;
			case Level::restore_health:
				m_actors.push_back(new RestoreHP(startX, startY, this));
				break;
			case Level::extra_life:
				m_actors.push_back(new ExtraLife(startX, startY, this));
				break;
			case Level::ammo:
				m_actors.push_back(new Ammo(startX, startY, this));
				break;
			}
		}
	}
	return 0;
}
int StudentWorld::objectType(int type)
{
	// wall -- returns -1
	if (type == IID_WALL)
		return -1;
	// Passable Objects -- returns 0
	if (type == IID_AMMO || type == IID_BULLET || type == IID_EXIT ||
		type == IID_EXTRA_LIFE || type == IID_HOLE || type == IID_JEWEL ||
		type == IID_RESTORE_HEALTH)
		return 0;
	// Damageable Objects -- returns 1
	if (type == IID_BOULDER || type == IID_PLAYER || type == IID_ANGRY_KLEPTOBOT ||
		type == IID_KLEPTOBOT || type == IID_SNARLBOT)
		return 1;
	// Factory
	if (type == IID_ROBOT_FACTORY)
		return 2;

	return -1;
}

void StudentWorld::setDisplayText()
{
	int score = getScore();	int level = getLevel(); int lives = getLives();
	int health = (getPlayer()->getHP() * 100) / 20;	
	int ammo = m_player->getAmmo();
	unsigned int bonus = getBonus();
	// Next, create a string from your statistics, of the form:
	// Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
	ostringstream oss;
	oss.fill('0');
	oss << "Score: " << setw(7) << score;
	oss << "  Level: " << setw(2) << level;
	oss.fill(' ');
	oss << "  Lives " << setw(2) << lives;
	oss << "  Health: " << setw(3) << health << "%";
	oss << "  Ammo: " << setw(3) << ammo;
	oss << "  Bonus: " << setw(4) << bonus;

	string s = oss.str();
	// Finally, update the display text at the top of the screen with your
	// newly created stats
	setGameStatText(s); // calls our provided GameWorld::setGameStatText
}
void StudentWorld::addActor(Actor *p)
{
	m_actors.push_back(p);
	return;
}
void StudentWorld::checkDeadActors()
{
	bool a;
	list<Actor *>::iterator i = m_actors.begin();
	while (i != m_actors.end())
	{
		a = false;
		Actor *p = *i;
		if (!p->isAlive())
		{
			a = true;
			i = m_actors.erase(i);
			delete p;
		}
		else i++;
	}
}

bool StudentWorld::isSpaceEmpty(int x, int y)
{
	list<Actor *>::const_iterator i;
	for (i = m_actors.begin(); i != m_actors.end(); i++)
	{
		Actor *p = *i;
		int curX = p->getX(); int curY = p->getY();
		if (x == curX && y == curY)
			return false;
	}
	return true;
}
bool StudentWorld::bulletCollision(Actor *bp, int x, int y, GraphObject::Direction dir)
{
	int destX = x, destY = y;
	if (isSpaceEmpty(destX, destY))
		return false; // Bullet does not collide

	Actor *p = getObjectAt(destX, destY, actors());
	list<Actor *> copy;
	int ptype = p->getItemType();
	if (objectType(ptype) == -1) // Bullet hits wall
	{		
		bp->setAlive(false);
		return false; // non interesting collision
	}
	if (objectType(ptype) == 0)	// Can pass object
	{
		Actor *sp = nullptr;
		int sptype;
		bool passable = spotPassable(destX, destY, p, m_actors);
		if (!passable)
		{
			list<Actor *>::iterator i;
			for (i = m_actors.begin(); i != m_actors.end(); ++i)
			{
				sp = *i;
				int spX = sp->getX(), spY = sp->getY();
				sptype = sp->getItemType();
				if (spX == destX && spY == destY)
					if (objectType(sptype) == 1)
					{
						sp->damage(sptype);
						bp->setAlive(false);
						return true; // interesting collision
					}
			}
		}
		return false;
	}
	if (objectType(ptype) == 1)	// Hits damageable object or boulder
	{
		p->damage(ptype);
		bp->setAlive(false);
		return true; // interesting collision
	}

	if (objectType(ptype) == 2)	// Special case, factory
	{
		list <Actor *> copy;
		Actor *p2 = getObjectWithout(destX, destY, p, actors(), copy);
		bool passable = (spotPassable(destX, destY, p2, copy));
		if (passable)
		{
			bp->setAlive(false);
			return false; // Bullet hits factory
		}
		int p2type = p2->getItemType();
		if (objectType(p2type) == 1) // Robot is on factory
		{
			p2->damage(p2type);
			bp->setAlive(false);
			return true; // interesting collision
		}
		bp->setAlive(false);
		return false;
	}
	return false;
}
bool StudentWorld::levelComplete()
{
	if (m_player->getX() == m_exitX && m_player->getY() == m_exitY && m_exitOpen)
	{
		increaseScore(2000 + m_bonus);
		return true; // Level completed
	}
	return false; // Level still incomplete
}

string StudentWorld::levelName()
{
	// Creates a string with the file name of the level
	ostringstream lvlNum;
	int level = getLevel();
	if (level >= 0 && level <= 9)
		lvlNum << "level0" << level << ".dat";
	else if (level > 9 && level <= 99)
		lvlNum << "level" << level << ".dat";
	return lvlNum.str();
}
Actor* StudentWorld::getObjectAt(int x, int y, list<Actor *> &actors)
{
	int playerX = m_player->getX(), playerY = m_player->getY();
	if (x == playerX && y == playerY) return m_player;

	list<Actor *>::const_iterator i;
	for (i = actors.begin(); i != actors.end(); ++i)
	{
		Actor *p = *i;
		int curX = p->getX(); int curY = p->getY();
		if (x == curX && y == curY)  // found object of interest
		{
			return p;
		}
	}
	return nullptr; // space is empty
}
Actor* StudentWorld::getObjectWithout(int x, int y, Actor *p, list<Actor *> &orig, list<Actor *> &copy)
{
	copy = orig;
	if (p == m_player)	return getObjectAt(x, y, copy);
	copy.remove(p);
	return getObjectAt(x, y, copy);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
