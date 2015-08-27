#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"

class Player;
class Actor;
#include "Level.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		:GameWorld(assetDir), bonus(1000), exitActive(false), levelFin(true), numJewels(0)
	{
	}

	virtual ~StudentWorld()
	{	
	}

	int loadLevel();
	virtual int init();
	virtual int move();
	void determineNext(int& x, int& y, GraphObject::Direction);
	bool isInGrid(int x, int y);
	std::list<Actor*> getOccupants(int x, int y);
	bool isWalkableLoc(int x, int y);
	bool isPlayerLoc(int x, int y);
	void addActor(Actor* ap);
	void removeDeadActors();
	void setDisplayText();

	int getBonus()
	{
		return bonus;
	}

	void decBonus()
	{
		if (bonus > 0) bonus--;
	}
	
	virtual void cleanUp();

	std::list<Actor*> getActors()
	{
		return m_actors;
	}

	Player* getPlayer()
	{
		return m_player;
	}

	void decNumJewels()
	{
		numJewels--;
	}

	bool getExitActive()
	{
		return exitActive;
	}

	void setLevelFin(bool val)
	{
		levelFin = val;
	}

private:
	std::list<Actor*> m_actors;
	Player* m_player;
	unsigned int bonus;
	bool exitActive, levelFin;
	unsigned int  numJewels;
	

};

#endif // STUDENTWORLD_H_
