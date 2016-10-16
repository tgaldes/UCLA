#include "StudentWorld.h"
#include "Level.h"
#include "Actor.h"
#include <list>
#include <string>

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <ctime>
using namespace std;

StudentWorld::StudentWorld()
{
	levelNum = 0;
	zumisRemaining = 0;
	m_finishedLevel = false;
	numberOfCurrentSprayers = 0;
	maxSprayers = 0;
	srand(static_cast<unsigned int>(time(0)));
}

GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

int StudentWorld::randInt(int lowest, int highest)
{
    if (highest < lowest)
        swap(highest, lowest);
    return lowest + (rand() % (highest - lowest + 1));
}

int StudentWorld::loadLevel()
{
	//erase brick array
	for(int i = 0; i < VIEW_HEIGHT; i++)
		for(int j = 0; j < VIEW_WIDTH; j++)
			bricks[j][i] = 0;
	string curLevel;
	
	zumisRemaining = 0;
	exitInvisible = true;
	m_finishedLevel = false;
	numberOfCurrentSprayers = 0;
	maxSprayers = 2;
	currentSprayerBoostDuration = 0;
	string levelFileName = "level";
	char two[256];
	sprintf(two, "%02d", getLevel());
	levelFileName += two;
	levelFileName += ".dat";
	cout << levelFileName << endl;

	

	Level lev;
	Level::LoadResult result = lev.loadLevel(levelFileName);
	if(result == Level::load_fail_file_not_found)
		return -1;

	if(result == Level::load_fail_bad_format)
		return -2;
	probOfGoodieOverall = lev.getOptionValue(optionProbOfGoodieOverall);
	probOfExtraLifeGoodie = lev.getOptionValue(optionProbOfExtraLifeGoodie);
	probOfMoreSprayersGoodie = lev.getOptionValue(optionProbOfMoreSprayersGoodie);
	probOfWalkThruGoodie = lev.getOptionValue(optionProbOfWalkThruGoodie);
	goodieLifetimeInTicks = lev.getOptionValue(optionGoodieLifetimeInTicks);
	maxSprayerDuration = lev.getOptionValue(optionBoostedSprayerLifetimeTicks);
	walkThroughDuration= lev.getOptionValue(optionWalkThruLifetimeTicks);
	newMaxSprayers = lev.getOptionValue(optionMaxBoostedSprayers);
	smellDistance = lev.getOptionValue(optionComplexZumiSearchDistance);
	startingBonus = lev.getOptionValue(optionLevelBonus);


	//put all actors in current level in the list of actors
	for(int x = 0; x < VIEW_WIDTH; x++)
	{
		for(int y = 0; y < VIEW_HEIGHT; y++)
		{

			Level::MazeEntry item = lev.getContentsOf(x, y);
			if(item == Level::perma_brick)
			{
				actors.push_front(new PermaBrick(x, y, this));
				bricks[x][VIEW_HEIGHT - y - 1] = 9;
			}
			else if(item == Level::destroyable_brick)
			{
				actors.push_front(new DestroyableBrick(x, y, this));
				bricks[x][VIEW_HEIGHT - y - 1] = 1;
			}
			else if(item == Level::player)
			{
				player = new Player(x, y, this);
				actors.push_front(player);
			}
			else if(item == Level::simple_zumi)
			{
				actors.push_front(new SimpleZumi(x, y, this, lev.getOptionValue(optionTicksPerSimpleZumiMove), randInt(0, 3)));
				zumisRemaining++;
			}
			else if(item == Level::complex_zumi)
			{
				actors.push_front(new ComplexZumi(x, y, this, lev.getOptionValue(optionTicksPerComplexZumiMove), randInt(0, 3)));
				zumisRemaining++;
			}
			else if(item == Level::exit)
			{
				exit = new Exit(x, y, this);
				actors.push_front(exit);
			}

		}
	}

	return 1;
}

int StudentWorld::move()
{
	if(maxSprayers > 2)
	{
		currentSprayerBoostDuration--;
		if(currentSprayerBoostDuration == 0)
			maxSprayers = 2;
	}
	list<Actor*>::iterator it = actors.begin();
	while(it != actors.end())
	{
		if(!(*it)->takeTurn())
		{
			actors.erase(it++);
		}
		else
			it++;
		if(!player->isAlive())
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
	}
	if(zumisRemaining == 0 && exitInvisible)
	{
		exit->setVisible(true);
		exit->setVisibility(true);
		exitInvisible = false;
	}
	
	if(startingBonus > 0)
		startingBonus--;
	setGameStatText(parseText());
	if(m_finishedLevel)
	{
		increaseScore(startingBonus);
		levelNum++;
		return GWSTATUS_FINISHED_LEVEL;
	}

	if(!player->isAlive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::operator->()
{return true;}

void StudentWorld::pushActor(Actor* a)
{
	actors.push_front(a);
}

void StudentWorld::movePlayer(int move, Player* me)
{
	if(move == KEY_PRESS_DOWN && me->getY() != 1 && !wallBlocking(me->getX(), me->getY() -1, me->canWalkThroughWalls()))
		me->moveTo(me->getX(), me->getY() -1);
	if(move == KEY_PRESS_LEFT && me->getX() != 1 && !wallBlocking(me->getX() - 1, me->getY(), me->canWalkThroughWalls()))
		me->moveTo(me->getX() - 1, me->getY());
	if(move == KEY_PRESS_UP && me->getY() != VIEW_HEIGHT - 2 && !wallBlocking(me->getX(), me->getY() +1, me->canWalkThroughWalls()))
		me->moveTo(me->getX(), me->getY() + 1);
	if(move == KEY_PRESS_RIGHT && me->getX() != VIEW_WIDTH - 2 && !wallBlocking(me->getX() + 1, me->getY(), me->canWalkThroughWalls()))
		me->moveTo(me->getX() + 1, me->getY());
	if(move == KEY_PRESS_SPACE && numberOfCurrentSprayers < maxSprayers)
	{
		actors.push_front(new BugSprayer(me->getX(), me->getY(), this));
		bricks[me->getX()][VIEW_HEIGHT - me->getY() - 1] = 5;
 		numberOfCurrentSprayers++;
	}
}

bool StudentWorld::wallBlocking(int x, int y, bool canMoveThroughWalls)
{
	list<Actor*>::iterator it = actors.begin();
	while(it != actors.end())
	{
		if((*it)->getX() == x && (*it)->getY() == y)
		{
			if((*it)->id == 9)
				return true;
			if((*it)->id == 10 && !canMoveThroughWalls)
				return true;
		}
		it++;
	}
	return false;
}



void StudentWorld::explode(Actor* a)
{
	if(a->isAlive())
	{
		a->setDead();
		a->setVisible(false);
		numberOfCurrentSprayers--;
		playSound(SOUND_SPRAY);
		actors.push_front(new BugSpray(a->getX(), a->getY(), this));
		bricks[a->getX()][VIEW_HEIGHT-a->getY() - 1] = 0;

		//moving right
		bool continueRight = true;

		if(bricks[a->getX() + 1][VIEW_HEIGHT - a->getY() - 1] == 0 || bricks[a->getX() + 1][VIEW_HEIGHT - a->getY() - 1] == 5)
		{
			actors.push_front(new BugSpray(a->getX() + 1, a->getY(), this));
		}
		else if(bricks[a->getX() + 1][VIEW_HEIGHT - a->getY() - 1] == 1)
		{
			continueRight = false;
			actors.push_front(new BugSpray(a->getX() + 1, a->getY(), this));
		}
		else if(bricks[a->getX() + 1][VIEW_HEIGHT - a->getY() - 1] == 9)
		{continueRight = false;}

		if(continueRight)
		{
			if(bricks[a->getX() + 2][VIEW_HEIGHT - a->getY() - 1] == 0  || bricks[a->getX() + 2][VIEW_HEIGHT - a->getY() - 1] == 5)
			{
				actors.push_front(new BugSpray(a->getX() + 2, a->getY(), this));
			}
			else if(bricks[a->getX() + 2][VIEW_HEIGHT - a->getY() - 1] == 1)
			{
				actors.push_front(new BugSpray(a->getX() + 2, a->getY(), this));
			}
		}
		


		//moving left
		bool continueLeft = true;
	
		if(bricks[a->getX() - 1][VIEW_HEIGHT - a->getY() - 1] == 0 || bricks[a->getX() - 1][VIEW_HEIGHT - a->getY() - 1] == 5)
		{
			actors.push_front(new BugSpray(a->getX() - 1, a->getY(), this));
		}
		else if(bricks[a->getX() - 1][VIEW_HEIGHT - a->getY() - 1] == 1)
		{
			continueLeft = false;
			actors.push_front(new BugSpray(a->getX() - 1, a->getY(), this));
		}
		else if(bricks[a->getX() - 1][VIEW_HEIGHT - a->getY() - 1] == 9)
		{continueLeft = false;}

		if(continueLeft)
		{
			if(bricks[a->getX() - 2][VIEW_HEIGHT - a->getY() - 1] == 0 || bricks[a->getX() - 2][VIEW_HEIGHT - a->getY() - 1] == 5)
			{
				actors.push_front(new BugSpray(a->getX() - 2, a->getY(), this));
			}
			else if(bricks[a->getX() - 2][VIEW_HEIGHT - a->getY() - 1] == 1)
			{
				actors.push_front(new BugSpray(a->getX()- 2, a->getY(), this));
			}
		}

		//moving up
		bool continueUp = true;

		if(bricks[a->getX()][VIEW_HEIGHT - a->getY() - 2] == 0 || bricks[a->getX()][VIEW_HEIGHT - a->getY() - 2] == 5)
		{
			actors.push_front(new BugSpray(a->getX(), a->getY() + 1, this));
		}
		else if(bricks[a->getX()][VIEW_HEIGHT - a->getY() - 2] == 1)
		{
			continueUp = false;
			actors.push_front(new BugSpray(a->getX() , a->getY() + 1, this));
		}
		else if(bricks[a->getX()][VIEW_HEIGHT - a->getY() - 2] == 9)
		{continueUp = false;}

		if(continueUp)
		{
		
			if(bricks[a->getX()][VIEW_HEIGHT - a->getY() - 3] == 0 || bricks[a->getX()][VIEW_HEIGHT - a->getY() - 3] == 5)
			{
				actors.push_front(new BugSpray(a->getX(), a->getY() + 2, this));
			}
			else if(bricks[a->getX()][VIEW_HEIGHT - a->getY() - 3] == 1)
			{
				actors.push_front(new BugSpray(a->getX(), a->getY() + 2, this));
			}
		}


		//moving down
		bool continueDown = true;

		if(bricks[a->getX()][VIEW_HEIGHT - a->getY()] == 0 || bricks[a->getX()][VIEW_HEIGHT - a->getY()] == 5)
		{
			actors.push_front(new BugSpray(a->getX(), a->getY() - 1, this));
		}
		else if(bricks[a->getX()][VIEW_HEIGHT - a->getY()] == 1)
		{
			continueDown = false;
			actors.push_front(new BugSpray(a->getX() , a->getY() - 1, this));
		}
		else if(bricks[a->getX()][VIEW_HEIGHT - a->getY()] == 9)
		{continueDown = false;}

		if(continueDown)
		{
			if(bricks[a->getX()][VIEW_HEIGHT - a->getY() + 1] == 0 || bricks[a->getX()][VIEW_HEIGHT - a->getY() + 1] == 5)
			{
				actors.push_front(new BugSpray(a->getX(), a->getY() - 2, this));
			}
			else if(bricks[a->getX()][VIEW_HEIGHT - a->getY() + 1] == 1)
			{
				actors.push_front(new BugSpray(a->getX(), a->getY() - 2, this));
			}
		}
	}
}

void StudentWorld::bugSprayTakeTurn(int x, int y)
{
	list<Actor*>::iterator item;
	for(int i = 0; i < 3; i++)
	{
		item = actors.begin();
		while(item != actors.end() && (((*item)->getX() != x || (*item)->getY() != y) || (*item)->id == 5))
		{
			item++;
		}

		if(item == actors.end())
			break;
		if((*item)->id == 10 || (*item)->id == 0 || (*item)->id == 2 || (*item)->id == 1)
			(*item)->setDead();
		if((*item)->id == 4 && (*item)->isAlive())
		{
			(*item)->getWorld().explode((*item));
		}
	}

}

void StudentWorld::checkIsZumiAlive(int x, int y, Actor* a)
{
	list<Actor*>::iterator it = actors.begin();
	while(true)
	{
		if(it == actors.end())
			break;
		if((*it) == a)
		{
			it++;
			continue;
		}

		if((*it)->id != 5)
		{
			it++;
			continue;
		}

		if((*it)->getX() != x)
		{
			it++;
			continue;
		}

		if((*it)->getY() != y)
		{
			it++;
			continue;
		}
		break;
	}

	if(it == actors.end())
		return;
	else
		a->setDead();
}

void StudentWorld::printBricks()
{
	for(int i = 0; i < VIEW_HEIGHT; i++)
	{
		for(int j = 0; j < VIEW_WIDTH; j++)
			cout << bricks[j][i];
		cout << endl;
	}
}

void StudentWorld::decrementZumis()
{
	zumisRemaining--;
}

Player* StudentWorld::getPlayer()
{
	return player;
}

void StudentWorld::finishedLevel()
{
	m_finishedLevel = true;
}

void StudentWorld::setMaxSprayers()
{
	maxSprayers = newMaxSprayers;
	currentSprayerBoostDuration+= maxSprayerDuration;
}

string StudentWorld::parseText()
{
	string toReturn = "Score:  ";
	char buffer[256]; 
	sprintf(buffer, "%08d", getScore());
	toReturn += buffer;

	toReturn += "  Level:  ";
	char two[256];
	sprintf(two, "%02d", getLevel());
	toReturn += two;

	toReturn += "  Lives:  ";
	char three[256];
	sprintf(three, "%03d", getLives());
	toReturn += three;

	toReturn += "  Bonus:  ";

	if(startingBonus > 99999);
	else if(startingBonus > 9999)
		toReturn += " ";
	else if(startingBonus > 999)
		toReturn += "  ";
	else if(startingBonus > 99)
		toReturn += "   ";
	else if(startingBonus > 9)
		toReturn += "    ";
	else
		toReturn += "     ";
	stringstream ss;
	ss << startingBonus;
	toReturn += ss.str();

	return toReturn;
}
void StudentWorld::cleanUp()
{
	list<Actor*>::iterator it = actors.begin();
	while(it != actors.end())
	{
		(*it)->setVisible(false);
		actors.erase(it++);
	}
	delete exit;
	delete player;
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
