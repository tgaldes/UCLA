#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <list>
using namespace std;
class Actor;
class Player;
class Exit;
class Level;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld();
	virtual ~StudentWorld();
	virtual int init()
    {
		int result = loadLevel();
		if(result == -1)
		{
			if(getLevel() != 0)
				return GWSTATUS_PLAYER_WON;
			else
				return GWSTATUS_NO_FIRST_LEVEL;
		}
		else if(result == -2)
			return GWSTATUS_LEVEL_ERROR;

		return GWSTATUS_CONTINUE_GAME;
    }

	bool operator->();
	void movePlayer(int move,Player* me);
	bool wallBlocking(int x, int y, bool canMoveThroughWalls);
	void pushActor(Actor* a);
	void explode(Actor* a);
	void bugSprayTakeTurn(int x, int y);
	void checkIsZumiAlive(int x, int y, Actor* a);
	void decrementZumis();
	void finishedLevel();
	virtual int move();

	virtual void cleanUp();

	int loadLevel();
	int bricks[VIEW_WIDTH][VIEW_HEIGHT];
	void printBricks();
	Player* getPlayer();
	int randInt(int lowest, int highest);
	void setMaxSprayers();
	int probOfGoodieOverall;
	int probOfWalkThruGoodie;
	int probOfMoreSprayersGoodie;
	int probOfExtraLifeGoodie;
	int goodieLifetimeInTicks;
	int walkThroughDuration;
	int maxSprayerDuration;
	int newMaxSprayers;
	int currentSprayerBoostDuration;
	int smellDistance;
	int startingBonus;
private:
	int levelNum;
	list<Actor*> actors;
	int zumisRemaining;
	Exit* exit;
	Player* player;
	bool exitInvisible;
	bool m_finishedLevel;
	int numberOfCurrentSprayers;
	int maxSprayers;
	string parseText();
	

};


#endif // STUDENTWORLD_H_
