#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

bool spotPassable(int destX, int destY, Actor *p, list<Actor *> &orig)
{
	if (p == nullptr)
		return true;
	Actor *p1 = p->getWorld()->getObjectAt(destX, destY, orig);
	int type = -1;
	if (p1 != nullptr) type = p1->getItemType();
	int px = p->getWorld()->objectType(type);
	if (px == -1 || px == 1 || px == 2) // Tries to walk into wall or impassable object
	{
		return false;
	}
	if (px == 0)	// Can pass object
	{
		list<Actor *> copy;
		Actor *p1 = p->getWorld()->getObjectWithout(destX, destY, p, orig, copy); // Additional objects
		return (true && spotPassable(destX, destY, p1, copy));
	}
	return false;
}


//////////////////////////////////////////////////////
//					doSomething()					//
//////////////////////////////////////////////////////

void Player::doSomething()
{
	if (!isAlive())	return; // player is dead!
	int ch;	int curX = getX(), curY = getY();
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_UP:
			characterMove(curX, curY + 1);
			setDirection(up);
			break;
		case KEY_PRESS_DOWN:
			characterMove(curX, curY - 1);
			setDirection(down);
			break;
		case KEY_PRESS_RIGHT:
			characterMove(curX + 1, curY);
			setDirection(right);
			break;
		case KEY_PRESS_LEFT:
			characterMove(curX - 1, curY);
			setDirection(left);
			break;
		case KEY_PRESS_SPACE:
			fireBullet();
			break;
		case KEY_PRESS_ESCAPE:
			setAlive(false);
			break;
		}
	}
		return;
}
void Bullet::doSomething()
{
	if (!isAlive())	return; // dead!
	int curX = getX(); int curY = getY();
	Direction dir = getDirection();

	// Check if bullet collides with anything	
	bool collides = getWorld()->bulletCollision(this, curX, curY, dir);
	moveInDir(dir);
	return;
}
void Exit::doSomething()
{
	if (getWorld()->numJewels() == 0 && !isVisible())
	{
		setVisible(true);
		getWorld()->setExitOpen();
		getWorld()->playSound(SOUND_REVEAL_EXIT);
	}
	return;
}
void Snarlbot::doSomething()
{
	if (getTicks() == -1) // Ticks have not been initialized
	{
		int level = getWorld()->getLevel();
		int x = (28 - level) / 4;
		if (x < 3)
			x = 3; // no SnarlBot moves more frequently than this
		initTicks(x - 1);
	}
	if (!isAlive()) return;
	if (getTicks() > 0) // Rest
	{
		decTicks(); return;
	}
	else if (getTicks() == 0)
	{
		int destX = getX(), destY = getY();
		Direction dir = getDirection(), opp;
		switch (dir)
		{
		case up:
			destY++; opp = down; break;
		case down:
			destY--; opp = up; break;
		case right:
			destX++; opp = left; break;
		case left:
			destX--; opp = right; break;
		}
		Actor *b = getWorld()->getObjectAt(destX, destY, getWorld()->actors());
		int type = -1;
		if (b != nullptr)
			type = b->getItemType();
		if (canFireAtPlayer()); // Returns true if robot can and subsequently does fire at player, else false
		else if (spotPassable(destX, destY, b, getWorld()->actors()) && type != IID_HOLE)
		{
			moveInDir(dir);	
		}
		else
		{
			setDirection(opp);
		}
		decTicks(); // resets ticks
	}
	return;
}
void Kleptobot::doSomething()
{
	// Initialize local variables
	int curX = getX(), curY = getY();
	int destX = curX, destY = curY;
	Direction dir = getDirection();
	switch (dir)
	{
	case up:	destY++; break;
	case down:	destY--; break;
	case right:	destX++; break;
	case left:	destX--; break;
	}
	Actor *b = getWorld()->getObjectAt(destX, destY, getWorld()->actors());
	int type = -1;
	if (b != nullptr)
	{
		type = b->getItemType();
	}

	Actor *ob = getWorld()->getObjectAt(curX, curY, getWorld()->actors());
	int obtype = ob->getItemType();
	if (getTicks() == -1) // Ticks have not been initialized
	{
		int level = getWorld()->getLevel();
		int x = (28 - level) / 4;
		if (x < 3)
			x = 3; // no Kleptobot moves more frequently than this
		initTicks(x - 1);
	}
	if (!isAlive())	return;


	if (getTicks() > 0) // Rest
	{
		decTicks(); return;
	}
	else if (getTicks() == 0) // We can do something
	{
		if (getItemType() == IID_ANGRY_KLEPTOBOT) // Check if I can shoot at player
		{
			if (canFireAtPlayer())
			{
				decTicks();
				return;
			}
		}
		// See if Kleptobot can collect an item
		if (obtype == IID_AMMO || obtype == IID_EXTRA_LIFE || obtype == IID_RESTORE_HEALTH)
		{
			// Kleptobot has 1/10 chance of collecting an item
			int collectChance = rand() % 10;
			if (collectChance == 0 && hasItem == false && ob->isVisible())
			{
				hasItem = true;
				ob->setVisible(false);
				item = ob;
				decTicks();
				getWorld()->playSound(SOUND_ROBOT_MUNCH);	
				return; // Do nothing more after collecting item
			}
		}
		// Check if we can move in a straight line
		if (m_distanceBeforeTurning == 0)
		{
			m_distanceBeforeTurning = rand() % 7 + 1;
			moveInRandDir();
			decTicks(); // resets ticks
		}
		else if (spotPassable(destX, destY, b, getWorld()->actors()) && type != IID_HOLE)
		{
			moveInDir(dir);
			if (hasItem) item->moveInDir(dir);
			decTicks();
			m_distanceBeforeTurning--;
		}
		// Hit an obstance or distance before turning = 0
		else if (!spotPassable(destX, destY, b, getWorld()->actors()) || type == IID_HOLE)
		{
			m_distanceBeforeTurning = rand() % 7 + 1;
			moveInRandDir();
			decTicks(); // resets ticks
		}
	}
}
void KBotFactory::doSomething()
{
	//cout << "Top Bound: " << topBound << endl;
	//cout << "Bot Bound: " << botBound << endl;
	//cout << "Right Bound: " << rightBound << endl;
	//cout << "Left Bound: " << leftBound << endl;
	botsInRegion = 0;
	botOnFactory = false;
	list<Actor *> actors = getWorld()->actors();
	// Process through entire region and count Kleptobots
	for (list<Actor *>::iterator i = actors.begin(); i != actors.end(); ++i)
	{
		// See if a Kleptobot is on this space
		Actor *p = *i;
		int pX = p->getX(), pY = p->getY();
		int type = -1;
		if (p != nullptr) type = p->getItemType();
		if (type == IID_KLEPTOBOT || type == IID_ANGRY_KLEPTOBOT)
		{
			if (pX >= botBound && pX <= topBound  && pY >= leftBound && pY <= rightBound)
			{
				botsInRegion++;
				//cout << "Increased!" << endl;
			}
			//cout << "Bots In Region: <" << botsInRegion << ">" << endl;
			if (pX == getX() && pY == getY()) // Check if bot on factory
			{
				//cout << "Bot on factory" << endl;
				botOnFactory = true;
			}
		}
	}
	if (botsInRegion < 3 && !botOnFactory)
	{
		int spawnChance = rand() % 50;
		if (spawnChance == 0)
		{
			if (spawnType == 0)
			{
				getWorld()->addActor(new Kleptobot(getX(), getY(), getWorld()));
			}
			else if (spawnType == 1)
			{
				getWorld()->addActor(new Kleptobot(getX(), getY(), getWorld(), IID_ANGRY_KLEPTOBOT, 8));
			}
			getWorld()->playSound(SOUND_ROBOT_BORN);
		}
	}
//	cout << "Finished doing something" << endl;
}
//////////////////////////////////////////////////

void Character::damage(int type)
{
	decHP();
	if (type == IID_PLAYER)
	{
		if (getHP() <= 0)
		{
			setAlive(false);
			getWorld()->playSound(SOUND_PLAYER_DIE);
		}
		else getWorld()->playSound(SOUND_PLAYER_IMPACT);
	}
	else if (type == IID_BOULDER)
	{
		if (getHP() <= 0) setAlive(false);
	}
	else if (type == IID_SNARLBOT)
	{
		if (getHP() <= 0)
		{
			setAlive(false);
			getWorld()->increaseScore(100);
			getWorld()->playSound(SOUND_ROBOT_DIE);
		}
		else
		{
			getWorld()->playSound(SOUND_ROBOT_IMPACT);
		}
	}

}
void Kleptobot::damage(int type)
{
	this->decHP();
	if (getHP() <= 0)
	{
		if (hasItem) this->item->setVisible(true);
		this->setAlive(false);
		if (this->getItemType() == IID_KLEPTOBOT)
			getWorld()->increaseScore(10);
		else if (getItemType() == IID_ANGRY_KLEPTOBOT)
			getWorld()->increaseScore(20);
		getWorld()->playSound(SOUND_ROBOT_DIE);
	}
	else getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

// Helper Functions

void Kleptobot::moveInRandDir()
{
	// Set possible dest coords
	int curX = getX(), curY = getY();

	int destXUp = curX, destYUp = curY + 1;
	int destXDown = curX, destYDown = curY - 1;
	int destXRight = curX + 1, destYRight = curY;
	int destXLeft = curX - 1, destYLeft = curY;

	// Set current movable directions
	int movableDirections = 4;

	// Check if spots in movable directions are passable
	bool spUP, spDOWN, spRIGHT, spLEFT; 
	StudentWorld *w = getWorld();
	list <Actor *> a = w->actors();

	Actor *aup = w->getObjectAt(destXUp, destYUp, a);
	Actor *adown = w->getObjectAt(destXDown, destYDown, a);
	Actor *aright = w->getObjectAt(destXRight, destYRight, a);
	Actor *aleft = w->getObjectAt(destXLeft, destYLeft, a);

	int auptype = -1, adowntype = -1, arighttype = -1, alefttype = -1;
	if (aup != nullptr) auptype = aup->getItemType();
	if (adown != nullptr) adowntype = adown->getItemType();
	if (aright != nullptr) arighttype = aright->getItemType();
	if (aleft != nullptr) alefttype = aleft->getItemType();
	
	spUP = (spotPassable(destXUp, destYUp, aup, a) && auptype != IID_HOLE);
	spDOWN = (spotPassable(destXDown, destYDown, adown, a) && adowntype != IID_HOLE);
	spRIGHT = (spotPassable(destXRight, destYRight, aright, a) && arighttype != IID_HOLE);
	spLEFT = (spotPassable(destXLeft, destYLeft, aleft, a) && alefttype != IID_HOLE);

	// Remove nonmovable directions from array
	Direction dirs[] = { up, down, left, right, none };
	Direction d;
	if (!spUP)
	{
		d = up;
		int i = 0;
		while (dirs[i] != none)
		{
			if (dirs[i] == d)
			{
				dirs[i] = dirs[movableDirections - 1];
				dirs[movableDirections - 1] = none;
				movableDirections--;
			}
			i++;
		}
	}
	if (!spDOWN)
	{
		d = down;
		int i = 0;
		while (dirs[i] != none)
		{
			if (dirs[i] == d)
			{
				dirs[i] = dirs[movableDirections - 1];
				dirs[movableDirections - 1] = none;
				movableDirections--;
			}
			i++;
		}
	}
	if (!spRIGHT)
	{
		d = right;
		int i = 0;
		while (dirs[i] != none)
		{
			if (dirs[i] == d)
			{
				dirs[i] = dirs[movableDirections - 1];
				dirs[movableDirections - 1] = none;
				movableDirections--;
			}
			i++;
		}
	}
	if (!spLEFT)
	{
		d = left;
		int i = 0;
		while (dirs[i] != none)
		{
			if (dirs[i] == d)
			{
				dirs[i] = dirs[movableDirections - 1];
				dirs[movableDirections - 1] = none;
				movableDirections--;
			}
			i++;
		}
	}
	if (movableDirections == 0)
	{
		int poss = rand() % 4;
		switch (poss)
		{
		case 0: setDirection(up); break;
		case 1: setDirection(down); break;
		case 2: setDirection(left); break;
		case 3: setDirection(right); break;
		}
		return;
	}
	int poss = rand() % movableDirections;
	Direction newdir;
	newdir = dirs[poss];
	moveInDir(newdir);
	setDirection(newdir);
	if (hasItem) item->moveInDir(newdir);	
	m_distanceBeforeTurning--;
}

bool Robot::canFireAtPlayer()
{
	int curX = getX(), curY = getY();
	Direction dir = getDirection();
	if (dir == up)
	{
		for (int y = curY + 1; y != VIEW_HEIGHT; y++)
		{
			Actor *b = getWorld()->getObjectAt(curX, y, getWorld()->actors());
			if (spotPassable(curX, y, b, getWorld()->actors()))
				continue;
			else
			{
				int type = b->getItemType();
				if (type == IID_PLAYER)
				{
					fireBullet();
					incAmmo(1);
					return true;
				}
				return false;
			}
		}
	}
	else if (dir == down)
	{
		for (int y = curY - 1; y != 0; y--)
		{
			Actor *b = getWorld()->getObjectAt(curX, y, getWorld()->actors());
			if (spotPassable(curX, y, b, getWorld()->actors()))
				continue;
			else
			{
				int type = b->getItemType();
				if (type == IID_PLAYER)
				{
					fireBullet();
					incAmmo(1);
					return true;
				}
				return false;
			}
		}
	}
	else if (dir == right)
	{
		for (int x = curX + 1; x != VIEW_WIDTH; x++)
		{
			Actor *b = getWorld()->getObjectAt(x, curY, getWorld()->actors());
			if (spotPassable(x, curY, b, getWorld()->actors()))
			{
				continue;
			}
			else
			{
				int type = b->getItemType();
				if (type == IID_PLAYER)
				{
					fireBullet();
					incAmmo(1);
					return true;
				}
				return false;
			}			
		}
	}
	else if (dir == left)
	{
		for (int x = curX - 1; x != 0; x--)
		{
			Actor *b = getWorld()->getObjectAt(x, curY, getWorld()->actors());
			if (spotPassable(x, curY, b, getWorld()->actors()))
				continue;
			else
			{
				int type = b->getItemType();
				if (type == IID_PLAYER)
				{
					fireBullet();
					incAmmo(1);
					return true;
				}
				return false;
			}
		}
	}
	return false;
}
void Character::fireBullet()
{
	if (getAmmo() == 0) return;
	decAmmo();
	int type = getItemType();
	if (type == IID_PLAYER) 
		getWorld()->playSound(SOUND_PLAYER_FIRE);
	else 
		getWorld()->playSound(SOUND_ENEMY_FIRE);
	Direction dir = getDirection();
	int curX = getX(); int curY = getY();
	StudentWorld *p = getWorld();
	switch (dir)
	{
	case up:
		p->addActor(new Bullet(curX, curY + 1, p, dir));
		break;
	case down:
		p->addActor(new Bullet(curX, curY - 1, p, dir));
		break;
	case right:
		p->addActor(new Bullet(curX + 1, curY, p, dir));
		break;
	case left:
		p->addActor(new Bullet(curX - 1, curY, p, dir));
		break;
	}
	return;
}
void Actor::initMembers(bool state, int imageID)
{
	setVisible(state);
	setAlive(true);
	m_itemType = imageID;
}
void Actor::moveInDir(Direction dir)
{
	int curX = getX(); int curY = getY();
	switch (dir)
	{
	case up:
		moveTo(curX, curY + 1);
		break;
	case down:
		moveTo(curX, curY - 1);
		break;
	case right:
		moveTo(curX + 1, curY);
		break;
	case left:
		moveTo(curX - 1, curY);
		break;
	}
}
int Character::characterMove(int destX, int destY)
{
	if (getWorld()->isSpaceEmpty(destX, destY)) // Empty Space
	{
		moveTo(destX, destY);
		return 0; // moved to empty space
	}
	else
	{
		Actor *b = getWorld()->getObjectAt(destX, destY, getWorld()->actors());
		int type = b->getItemType();
		bool passable = spotPassable(destX, destY, b, getWorld()->actors());
		if (passable && type != IID_HOLE)
		{
			moveTo(destX, destY);
			if (type == IID_AMMO || type == IID_EXTRA_LIFE || type == IID_JEWEL || type == IID_RESTORE_HEALTH)
				collectItem(type, b);
			return 1; // item collected
		}
		else	// Check for boulders
		{
			Direction dir;
			int x = getX(), y = getY();
			if (destX == x + 1) dir = up;
			else if (destX == x - 1) dir = down;
			else if (destY == y + 1) dir = right;
			else if (destY == y - 1) dir = left;
			if (type == IID_BOULDER)
			{
				int bdestX = destX, bdestY = destY;
				switch (dir)
				{
				case up:
					bdestX++;
					break;
				case down:
					bdestX--;
					break;
				case right:
					bdestY++;
					break;
				case left:
					bdestY--;
					break;
				}
				Actor *p = getWorld()->getObjectAt(bdestX, bdestY, getWorld()->actors());
				if (getWorld()->isSpaceEmpty(bdestX, bdestY))
				{
					b->moveTo(bdestX, bdestY);
					moveTo(destX, destY);
					return 2; // boulder pushed
				}				
				else if (p->getItemType() == IID_HOLE)
				{
					b->setAlive(false);
					p->setAlive(false);
					moveTo(destX, destY);
				}
				return -1; // Could not move
			}
			return -1; // could not move
		}
	}
}
void Character::collectItem(int type, Actor *item)
{
	if (!item->isAlive() || !item->isVisible())
		return;
	switch (type)
	{
	case IID_AMMO:
		incAmmo(20);
		getWorld()->increaseScore(100);
		item->setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		break;
	case IID_EXTRA_LIFE:
		getWorld()->incLives();
		getWorld()->increaseScore(1000);
		item->setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		break;
	case IID_JEWEL:
		getWorld()->decJewels();
		getWorld()->increaseScore(50);
		item->setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		break;
	case IID_RESTORE_HEALTH:
		incHP(20 - getHP());
		getWorld()->increaseScore(500);
		item->setAlive(false);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		break;
	}
}