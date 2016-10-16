


#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
using namespace std;

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:
	Actor(int image, int x, int y, StudentWorld* pt);
	bool isAlive() const;
	StudentWorld& getWorld();
	virtual void checkIsAlive() = 0;
	virtual bool takeTurn() = 0;
	void setDead();
	int id;
private:
	bool m_isAlive;
	StudentWorld* world;

};

class Brick : public Actor
{
public:
	Brick(int image, int x, int y, StudentWorld* pt);
	virtual void checkIsAlive();
	virtual bool takeTurn();
};

class PermaBrick : public Brick
{
public:
	PermaBrick(int x, int y, StudentWorld* pt);
};

class DestroyableBrick : public Brick
{
public:
	DestroyableBrick(int x, int y, StudentWorld* pt);
};

class Player : public Actor
{
public:
	Player(int x, int y, StudentWorld* pt);
	virtual bool takeTurn();
	virtual void checkIsAlive();
	bool canWalkThroughWalls() const;
	void setWalkThroughWalls(int ticks);
private:
	bool m_canWalkThroughWalls;
	int walkThroughWallsTicks;
	int maxSprayerTicks;
};

class Zumi : public Actor
{
public:
	Zumi(int x, int y, int image, StudentWorld* pt, int ticks, int dir);
	virtual bool takeTurn() = 0;
	void checkIsAlive();
	int ticksPerMove;
	int ticksUntilNextMove;
	int m_dir;
};

class SimpleZumi : public Zumi
{
public:
	SimpleZumi(int x, int y, StudentWorld* pt, int ticks, int dir);
	virtual bool takeTurn();
};

class ComplexZumi : public Zumi
{
public:
	ComplexZumi(int x, int y, StudentWorld* pt, int ticks, int dir);
	virtual bool takeTurn();
};

class BugSprayer : public Actor
{
public:
	BugSprayer(int x, int y, StudentWorld* pt);
	bool takeTurn();
	virtual void checkIsAlive();
	void explode();
private:
	int m_ticksRemaining;
};


class BugSpray : public Actor
{
public:
	BugSpray(int x, int y, StudentWorld* pt);
	virtual bool takeTurn();
	virtual void checkIsAlive();
private:
	int m_ticksRemaining;
};

class Exit : public Actor
{
public:
	Exit(int x, int y, StudentWorld* pt);
	virtual bool takeTurn();
	virtual void checkIsAlive();
	bool isVisible();
	void setVisibility(bool input);
private:
	bool m_isVisible;
};

class Goodie : public Actor
{
public:
	Goodie(int x, int y, int image, int ticksRemaining, StudentWorld* pt, int duration, int newMax);
	virtual bool takeTurn();
	virtual void checkIsAlive();
	int ticksTillDeath;
	int longLasting;
	int newMaxSprayers;
};






#endif // ACTOR_H_
