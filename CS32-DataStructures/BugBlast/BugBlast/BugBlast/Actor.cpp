#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <queue>


using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp


Actor::Actor(int image, int x, int y, StudentWorld* pt) : GraphObject(image, x, y)
{
	setVisible(true);
	m_isAlive = true;
	int ID = image;
	world = pt;
	id = image;
}

bool Actor::isAlive() const
{
	return m_isAlive;
}

StudentWorld& Actor::getWorld()
{
	return *world;
}

void Actor::setDead()
{
	m_isAlive = false;
}


Brick::Brick(int image, int x, int y, StudentWorld* pt) : Actor(image, x, y, pt)
{

}

bool Brick::takeTurn()
{
	if(!isAlive())
	{
		setVisible(false);
		getWorld().bricks[getX()][VIEW_HEIGHT - getY() - 1] = 0;
	return isAlive();
	}
}


PermaBrick::PermaBrick(int x, int y, StudentWorld* pt) : Brick(IID_PERMA_BRICK, x, y, pt)
{

}

void Brick::checkIsAlive()
{
}

DestroyableBrick::DestroyableBrick(int x, int y, StudentWorld* pt) : Brick(IID_DESTROYABLE_BRICK, x, y, pt)
{

}


Player::Player(int x, int y, StudentWorld* pt) : Actor(IID_PLAYER, x, y, pt)
{
	m_canWalkThroughWalls = false;
	walkThroughWallsTicks = 0;
	maxSprayerTicks = 0;
}

void Player::checkIsAlive()
{
	if(getWorld().bricks[getX()][VIEW_HEIGHT - getY() - 1] != 0 && !m_canWalkThroughWalls)
		if(getWorld().bricks[getX()][VIEW_HEIGHT - getY() - 1] != 5)
		{
		setDead();
		return;
		}
	//NEED TO CHECK S=ZUMIS
}

bool Player::canWalkThroughWalls() const
{
	return m_canWalkThroughWalls;
}

bool Player::takeTurn()
{
	checkIsAlive();
	if(!isAlive())
		return false;

	if(m_canWalkThroughWalls)
	{
		walkThroughWallsTicks--;
		if(walkThroughWallsTicks < 0)
			m_canWalkThroughWalls = false;
	}
	
	int ch;
	if(getWorld().getKey(ch))
	{
		getWorld().movePlayer(ch, this);
	}
	return isAlive();
}

void Player::setWalkThroughWalls(int ticks)
{
	walkThroughWallsTicks+=ticks;
	m_canWalkThroughWalls = true;
}

Zumi::Zumi(int x, int y, int image, StudentWorld* pt, int ticks, int dir) : Actor(image, x, y, pt)
{
	ticksPerMove = ticks;
	m_dir = dir;
	ticksUntilNextMove = 0;
}

void Zumi::checkIsAlive() 
{
	getWorld().checkIsZumiAlive(getX(), getY(), this);
}

SimpleZumi::SimpleZumi(int x, int y, StudentWorld* pt, int ticks, int dir) : Zumi (x, y, IID_SIMPLE_ZUMI, pt, ticks, dir)
{}

bool SimpleZumi::takeTurn()
{
	checkIsAlive();
	if(!isAlive())
	{
		setVisible(false);
		getWorld().decrementZumis();
		getWorld().playSound(SOUND_ENEMY_DIE);
		getWorld().increaseScore(100);

		//goodies
		int random = getWorld().randInt(0, 99);
		if(random < getWorld().probOfGoodieOverall)
		{
			if(getWorld().randInt(0, 99) < getWorld().probOfExtraLifeGoodie)
			{
				getWorld().pushActor(new Goodie(getX(), getY(), IID_EXTRA_LIFE_GOODIE, getWorld().goodieLifetimeInTicks, &getWorld(), 0, 0));
				
			}
			else if(getWorld().randInt(0, 99 - getWorld().probOfExtraLifeGoodie) < getWorld().probOfMoreSprayersGoodie)
			{
				getWorld().pushActor(new Goodie(getX(), getY(), IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, getWorld().goodieLifetimeInTicks, &getWorld(), getWorld().maxSprayerDuration, getWorld().newMaxSprayers));
			
			}
			else
			{
				getWorld().pushActor(new Goodie(getX(), getY(), IID_WALK_THRU_GOODIE, getWorld().goodieLifetimeInTicks, &getWorld(), getWorld().walkThroughDuration, 0));
				//cout << getWorld().walkThroughDuration << endl;
				
			}
		}
		return false;
	}

	//check to see if it is on the same square as the player
	if(getX() == getWorld().getPlayer()->getX() && getY() == getWorld().getPlayer()->getY() )
		getWorld().getPlayer()->setDead();
	
	if(ticksUntilNextMove == 0)
	{
		ticksUntilNextMove = ticksPerMove;

		//north
		if(m_dir == 0 && getWorld().bricks[getX()][VIEW_HEIGHT - getY() - 2] == 0)
		{
			moveTo(getX(), getY() + 1);
		}

		//east
		else if(m_dir == 1 && getWorld().bricks[getX() + 1][VIEW_HEIGHT - getY() - 1] == 0)
		{
			moveTo(getX() + 1, getY());
		}

		//south
		else if(m_dir == 2 && getWorld().bricks[getX()][VIEW_HEIGHT - getY()] == 0)
		{
			moveTo(getX(), getY() - 1);
		}

		//west
		else if(m_dir == 3 && getWorld().bricks[getX() - 1][VIEW_HEIGHT - getY() - 1] == 0)
		{
			moveTo(getX() - 1, getY());
		}

		else
			m_dir = getWorld().randInt(0, 3);

	}
	else
		ticksUntilNextMove--;

	if(getX() == getWorld().getPlayer()->getX() && getY() == getWorld().getPlayer()->getY() )
		getWorld().getPlayer()->setDead();

	return isAlive();
}

ComplexZumi::ComplexZumi(int x, int y, StudentWorld* pt, int ticks, int dir) : Zumi(x, y, IID_COMPLEX_ZUMI, pt, ticks,  dir)
{
}

bool ComplexZumi::takeTurn()
{
	checkIsAlive();
	if(!isAlive())
	{
		setVisible(false);
		getWorld().decrementZumis();
		getWorld().playSound(SOUND_ENEMY_DIE);
		getWorld().increaseScore(500);

		//goodies
		int random = getWorld().randInt(0, 99);
		if(random < getWorld().probOfGoodieOverall)
		{
			if(getWorld().randInt(0, 99) < getWorld().probOfExtraLifeGoodie)
			{
				getWorld().pushActor(new Goodie(getX(), getY(), IID_EXTRA_LIFE_GOODIE, getWorld().goodieLifetimeInTicks, &getWorld(), 0, 0));
				
			}
			else if(getWorld().randInt(0, 99 - getWorld().probOfExtraLifeGoodie) < getWorld().probOfMoreSprayersGoodie)
			{
				getWorld().pushActor(new Goodie(getX(), getY(), IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, getWorld().goodieLifetimeInTicks, &getWorld(), getWorld().maxSprayerDuration, getWorld().newMaxSprayers));
				
			}
			else
			{
				getWorld().pushActor(new Goodie(getX(), getY(), IID_WALK_THRU_GOODIE, getWorld().goodieLifetimeInTicks, &getWorld(), getWorld().walkThroughDuration, 0));
				
			}
		}
		return false;
	}

	//check to see if it is on the same square as the player
	if(getX() == getWorld().getPlayer()->getX() && getY() == getWorld().getPlayer()->getY() )
		getWorld().getPlayer()->setDead();

	if(ticksUntilNextMove == 0)
	{
		ticksUntilNextMove = ticksPerMove;
		bool canSmell = false;
		//smell distance computation
		if(getX() - getWorld().getPlayer()->getX() < getWorld().smellDistance && getX() - getWorld().getPlayer()->getX() > -getWorld().smellDistance  && getY() - getWorld().getPlayer()->getY() < getWorld().smellDistance && getY() - getWorld().getPlayer()->getY() > -getWorld().smellDistance)
		{
			canSmell = true;
		}

		bool foundPlayer = false;
		int moveDirection;
		if(canSmell)
		{
			queue<int> coordsX;
			queue<int> coordsY;
			queue<int> dir;
			int tempMap[VIEW_WIDTH][VIEW_HEIGHT];
			for(int i = 0; i < VIEW_WIDTH; i++)
				for(int j = 0; j < VIEW_HEIGHT; j++)
					tempMap[j][i] = getWorld().bricks[i][j];
			 
			coordsX.push(getX());
			coordsY.push(getY());
			dir.push(5);
			int loopCount = 0;
			tempMap[VIEW_HEIGHT - coordsY.front() - 1][coordsX.front()] = 5;
			int endX = getWorld().getPlayer()->getX();
			int endY = getWorld().getPlayer()->getY();
			
			
			while(!coordsX.empty() && !foundPlayer)
			{
				if(coordsX.front() == endX && coordsY.front() == endY)
				{
					foundPlayer = true;
				}
				//look west
				if(tempMap[VIEW_HEIGHT - coordsY.front() - 1][coordsX.front() - 1] == 0)
				{
					if(loopCount == 0)
						dir.push(3);
					else
						dir.push(dir.front());

					coordsX.push(coordsX.front() - 1);
					coordsY.push(coordsY.front());
					tempMap[VIEW_HEIGHT - coordsY.front() - 1][coordsX.front() - 1] = 5;
				}

				//look east
				if(tempMap[VIEW_HEIGHT - coordsY.front() - 1][coordsX.front() + 1] == 0)
				{
					if(loopCount == 0)
						dir.push(1);
					else
						dir.push(dir.front());

					coordsX.push(coordsX.front() + 1);
					coordsY.push(coordsY.front());
					tempMap[VIEW_HEIGHT - coordsY.front() - 1][coordsX.front() + 1] = 5;
				}

				//look south
				if(tempMap[VIEW_HEIGHT - coordsY.front()][coordsX.front()] == 0)
				{
					if(loopCount == 0)
						dir.push(2);
					else
						dir.push(dir.front());

					coordsX.push(coordsX.front());
					coordsY.push(coordsY.front() - 1);
					tempMap[VIEW_HEIGHT - coordsY.front()][coordsX.front()] = 5;
				}

				//look north
				if(tempMap[VIEW_HEIGHT - coordsY.front() - 2][coordsX.front()] == 0)
				{
					if(loopCount == 0)
						dir.push(0);
					else
						dir.push(dir.front());

					coordsX.push(coordsX.front());
					coordsY.push(coordsY.front() + 1);
					tempMap[VIEW_HEIGHT - coordsY.front() - 2][coordsX.front()] = 5;
				}

				loopCount++;
				coordsX.pop();
				coordsY.pop();
				moveDirection = dir.front();
				if(!dir.empty())
					dir.pop();

			}//end while loop
			
		}//end algorithim to determine move direction if player is within smelling distance

		if(foundPlayer)
		{
			if(moveDirection == 0)
			{
				moveTo(getX(), getY() + 1);
				m_dir = moveDirection;
			}
			if(moveDirection == 1)
			{
				moveTo(getX() + 1, getY());
				m_dir = moveDirection;
			}
			if(moveDirection == 2)
			{
				moveTo(getX(), getY() - 1);
				m_dir = moveDirection;
			}
			if(moveDirection == 3)
			{
				moveTo(getX() - 1, getY());
				m_dir = moveDirection;
			}
		}

		else//if can't smell the player
		{
			if(m_dir == 0 && getWorld().bricks[getX()][VIEW_HEIGHT - getY() - 2] == 0)
			{
				moveTo(getX(), getY() + 1);
			}

			//east
			else if(m_dir == 1 && getWorld().bricks[getX() + 1][VIEW_HEIGHT - getY() - 1] == 0)
			{
				moveTo(getX() + 1, getY());
			}

			//south
			else if(m_dir == 2 && getWorld().bricks[getX()][VIEW_HEIGHT - getY()] == 0)
			{
				moveTo(getX(), getY() - 1);
			}

			//west
			else if(m_dir == 3 && getWorld().bricks[getX() - 1][VIEW_HEIGHT - getY() - 1] == 0)
			{
				moveTo(getX() - 1, getY());
			}

		else
			m_dir = getWorld().randInt(0, 3);
	}

	}
	else
		ticksUntilNextMove--;

	if(getX() == getWorld().getPlayer()->getX() && getY() == getWorld().getPlayer()->getY() )
		getWorld().getPlayer()->setDead();

	return isAlive();
}

BugSprayer::BugSprayer(int x, int y, StudentWorld* pt) : Actor(IID_BUGSPRAYER, x, y, pt)
{
	m_ticksRemaining = 40;
}

bool BugSprayer::takeTurn()
{
	if(m_ticksRemaining <= 0)
	{
		setVisible(false);
		getWorld().explode(this);
		return false;
	}
	if(!isAlive())
		return false;
	
	m_ticksRemaining--;
	return true;
}


void BugSprayer::checkIsAlive()
{
	if(m_ticksRemaining <= 0)
		setDead();
}

BugSpray::BugSpray(int x, int y, StudentWorld* pt) : Actor(IID_BUGSPRAY, x, y, pt)
{
	m_ticksRemaining = 3;
}

bool BugSpray::takeTurn()
{
	checkIsAlive();

	//needs to delete itself as well
	if(!isAlive())
	{
		setVisible(false);
		return false;
	}

	getWorld().bugSprayTakeTurn(getX(), getY());
	m_ticksRemaining--;
	return true;
}

void BugSpray::checkIsAlive()
{
	if(m_ticksRemaining <= 0)
		setDead();
}

Exit::Exit(int x, int y, StudentWorld* pt) : Actor(3, x, y, pt)
{
	setVisible(false);
	m_isVisible = false;
}

bool Exit::takeTurn()
{
	if(m_isVisible)
	{
		if(getWorld().getPlayer()->getX() == getX() && getWorld().getPlayer()->getY() == getY())
		{
			getWorld().playSound(SOUND_FINISHED_LEVEL);
			getWorld().finishedLevel();
		}
	}
	return true;
}

void Exit::checkIsAlive()
{
}

void Exit::setVisibility(bool input)
{
	m_isVisible = input;
	if(m_isVisible)
		getWorld().playSound(SOUND_REVEAL_EXIT);
}

bool Exit::isVisible()
{
	return m_isVisible;
}

Goodie::Goodie(int x, int y, int image, int ticksRemaining, StudentWorld* pt, int duration, int newMax) : Actor(image, x, y, pt)
{
	ticksTillDeath = ticksRemaining;
	longLasting = duration;
	newMaxSprayers = newMax;
}

bool Goodie::takeTurn()
{
	checkIsAlive();
	if(!isAlive())
	{
		setVisible(false);
		return false;
	}
	ticksTillDeath--;

	if(getX() == getWorld().getPlayer()->getX() && getY() == getWorld().getPlayer()->getY() )
	{
		getWorld().playSound(SOUND_GOT_GOODIE);
		getWorld().increaseScore(1000);
		//extra life goodie
		if(id == IID_EXTRA_LIFE_GOODIE)
		{
			getWorld().incLives();
			setDead();
		}

		//walk through walls goodie
		if(id == IID_WALK_THRU_GOODIE)
		{
			getWorld().getPlayer()->setWalkThroughWalls(longLasting);
			setDead();
		}

		//max sprayers goodie
		if(id == IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE)
		{
			getWorld().setMaxSprayers();
			setDead();
		}
	}
	return true;
}

void Goodie::checkIsAlive()
{
	if(ticksTillDeath == 0)
		setDead();
}

