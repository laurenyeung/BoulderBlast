#include "StudentWorld.h"
#include "Actor.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <list>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::loadLevel()
{
	ostringstream oss;
	oss << "level";
	oss.fill('0');
	oss << setw(2) << getLevel() << ".dat";
	std::string curLevel = oss.str(); //get file name

	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(curLevel);

	if (result == Level::load_fail_file_not_found) return GWSTATUS_PLAYER_WON; //reached end of levels
	if ( result == Level::load_fail_bad_format) return GWSTATUS_LEVEL_ERROR; //wrong name format for file
	
	Level::MazeEntry item;
	for (int y = 0; y < VIEW_HEIGHT; y++)
	{
		for (int x = 0; x < VIEW_WIDTH; x++) //traverse entire grid of data file
		{
			item = lev.getContentsOf(x, y);
			switch (item) //initialize each item
			{
			case Level::empty:
			{
				cerr << "Location " << x << ", " << y << " is empty\n";
				break;
			}
			case Level::jewel:
			{
				cerr << "Location " << x << ", " << y << " is jewel\n";
				numJewels++;
				m_actors.push_back(new Jewel(IID_JEWEL, x, y, this)); //add to list of actors with a pointer to a new object
				break;
			}
			case Level::exit:
			{
				cerr << "Location " << x << ", " << y << " is exit\n";
				m_actors.push_back(new Exit(IID_EXIT, x, y, this));
				break;
			}
			case Level::boulder:
			{
				cerr << "Location " << x << ", " << y << " is boulder\n";
				m_actors.push_back(new Boulder(IID_BOULDER, x, y, this));
				break;
			}
			case Level::hole:
			{
				cerr << "Location " << x << ", " << y << " is hole\n";
				m_actors.push_back(new Hole(IID_HOLE, x, y, this));
				break;
			}
			case Level::extra_life:
			{
				cerr << "Location " << x << ", " << y << " is extralife\n";
				m_actors.push_back(new ELGoodie(IID_EXTRA_LIFE,x,y,this));
				break;
			}
				
			case Level::restore_health:
			{
				cerr << "Location " << x << ", " << y << " is restorehealth\n";
				m_actors.push_back(new RHGoodie(IID_RESTORE_HEALTH, x, y, this));
				break;
			}
				
			case Level::ammo:
			{
				cerr << "Location " << x << ", " << y << " is ammo\n";
				m_actors.push_back(new AGoodie(IID_AMMO, x, y, this));
				break;
			}
				
			case Level::player:
			{
				cerr << "Location " << x << ", " << y << " is player\n";
				m_player = new Player(IID_PLAYER, x, y, this, GraphObject::right); //pointer to world
				break;
			}
				
			case Level::horiz_snarlbot:
			{
				cerr << "Location " << x << ", " << y << " is hsnarl\n";
				m_actors.push_back(new SnarlBot(IID_SNARLBOT, x, y, this, GraphObject::right));
				break;
			}
				
			case Level::vert_snarlbot:
			{
				cerr << "Location " << x << ", " << y << " is vsnarl\n";
				m_actors.push_back(new SnarlBot(IID_SNARLBOT, x, y, this, GraphObject::down));	
				break;
			}
				
			case Level::kleptobot_factory:
			{
				cerr << "Location " << x << ", " << y << " is kleptFact\n";
				m_actors.push_back(new Factory(IID_ROBOT_FACTORY, x, y, this, true));
				break;
			}
			case Level::angry_kleptobot_factory:
			{
				cerr << "Location " << x << ", " << y << " is angryKleptFact\n";
				m_actors.push_back(new Factory(IID_ROBOT_FACTORY, x, y, this, false));
				break;
			}
			case Level::wall:
			{
				cerr << "Location " << x << ", " << y << " is wall\n";
				Actor* w = new Wall(IID_WALL, x, y, this, GraphObject::none);
				m_actors.push_back(w);
				break;
			}
				
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::init()
{
	if (getLevel() > 99) return GWSTATUS_PLAYER_WON;	//over level 99
	exitActive = false;
	levelFin = false;
	bonus = 1000;
	numJewels = 0;
	return loadLevel();
}

int StudentWorld::move()
{	

	list<Actor*>::iterator itr = m_actors.begin(); //make iterator
	m_player->doSomething();//player acts
	while (itr != m_actors.end()) //for each actor
	{
		Actor* cur = *itr;
		if (cur->getKeepable()) // actor is alive
			cur->doSomething(); // acts
		if (!(getPlayer()->getKeepable())) //if player is dead
		{
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		if (levelFin) //if completed level
		{
			return GWSTATUS_FINISHED_LEVEL;
		}
		itr++; //go to next actor
	}
	//everyone has acted
	removeDeadActors(); //remove actors who died during this tick
	if (!(getPlayer()->getKeepable())) //if player is dead
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (levelFin) //if completed level
	{
		return GWSTATUS_FINISHED_LEVEL;
	}
	decBonus(); //decrement possible bonus points
	if (numJewels == 0) exitActive = true; //exit now can be visible
	setDisplayText(); //text at top
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::determineNext(int& x, int&y, GraphObject::Direction d)
{
	int dx = 0, dy = 0;
	switch (d) //determine change in x,y loc based on direction
	{
	case GraphObject::up:
	{
		dy = 1;
		break;
	}
	case GraphObject::down:
	{
		dy = -1;
		break;
	}
	case GraphObject::left:
	{
		dx = -1;
		break;
	}
	case GraphObject::right:
	{
		dx = 1;
		break;
	}
	}
	x += dx; //add changes to vals passed by reference
	y += dy;
	
}

bool StudentWorld::isInGrid(int x, int y)
{
	if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) return false;
	return true;
}

std::list<Actor*> StudentWorld::getOccupants(int x, int y)
{
	std::list<Actor*> occupants;
	if (!(isInGrid(x, y))) return occupants;
	std::list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end()) //loop through all actors on grid
	{
		if (x == (*it)->getX() && y == (*it)->getY()) //if on same location
		{
			occupants.push_back(*it);
		}
		it++; //not modifying original data so just iterate normally
	}
	return occupants;
}

bool StudentWorld::isWalkableLoc(int x, int y) //FIX TO ACCOMODATE isObstruct. interactions?
{
	if (!(isInGrid(x, y))) return false;
	std::list<Actor*> occupants = getOccupants(x, y);
	std::list<Actor*>::iterator it = occupants.begin();
	while (it != occupants.end())
	{
		if ((*it)->getBlocksPlayer()){ //obstructs player from moving onto square
			 return false;
		}
		it++;
	}
	return true;
}

bool StudentWorld::isPlayerLoc(int x, int y)
{
	if (x == m_player->getX() && y == m_player->getY()) return true;
	return false;
}

void StudentWorld::addActor(Actor* ap)
{
	m_actors.push_front(ap);
}


void StudentWorld::removeDeadActors()
{
	std::list<Actor*>::iterator it = m_actors.begin();
	Actor* temp;
	while (it != m_actors.end())
	{
		if ((*it)->getKeepable() == false) //not alive
		{
			temp = *it; 
			it = m_actors.erase(it); //remove from list and link to next element
			delete temp; //deallocate memory
			continue;
		}
		it++; //increment if is not dead
	}
}


void StudentWorld::setDisplayText()
{
	int score = getScore();
	int level = getLevel();
	unsigned int bonus = getBonus();
	int livesLeft = getLives();
	double healthp = (m_player->getHitPoints() / 20.0)*100; //maxplayer health

	ostringstream oss;
	oss << "Score: ";
	oss.fill('0');
	oss << setw(7) << score << "  ";
	oss << "Level: ";
	oss.fill('0');
	oss << setw(2) << level << "  ";
	oss << "Lives: ";
	oss.fill(' ');
	oss << setw(2) << livesLeft << "  ";
	oss << "Health: ";
	oss.fill(' ');
	oss.setf(ios::fixed);
	oss.precision(0);
	oss << setw(3) << healthp << "%  ";
	oss << "Ammo: ";
	oss.fill(' ');
	oss << setw(3) << m_player->getAmmo() << "  ";
	oss << "Bonus: ";
	oss.fill(' ');
	oss << setw(4) << bonus;
	string text = oss.str();
	setGameStatText(text); // calls our provided GameWorld::setGameStatText
}

void StudentWorld::cleanUp()
{
	std::list<Actor*>::iterator it = m_actors.begin();
	Actor* temp;
	while (it != m_actors.end())
	{
		temp = (*it);
		it = m_actors.erase(it);
		delete temp;
	}
	delete m_player;
}