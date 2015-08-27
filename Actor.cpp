#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <list>
#include <iostream>
#include <cstdlib>
#include <set>

void Player::doSomething()
{
	if (!(getKeepable())) return; //dead, so return
	int ch;
	bool shoot = false;
	if (getWorld()->getKey(ch)) // key pressed
	{
		switch (ch)
		{
		case KEY_PRESS_ESCAPE:
		{
			setHitPoints(0);
			return;
			break;
		}
		case KEY_PRESS_SPACE:
		{
			shoot = true;
			break;
		}
		//move keys determine player's direction
		case KEY_PRESS_LEFT:
		{
			setDirection(GraphObject::left);
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(GraphObject::right);
			break;
		}
		case KEY_PRESS_DOWN:
		{
			setDirection(GraphObject::down);
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(GraphObject::up);
			break;
		}
		}

		
		int newLocX = getX(), newLocY = getY();
		getWorld()->determineNext(newLocX, newLocY, getDirection()); //find next location in that direction

		if (shoot) //place bullet in that location
		{
			if (ammo <= 0) return;
			getWorld()->addActor(new Bullet(IID_BULLET, newLocX, newLocY, getWorld(), getDirection()));
			getWorld()->playSound(SOUND_PLAYER_FIRE);
			ammo--;
			return;
		}

		//otherwise is players next location
		int adjLocX = newLocX, adjLocY = newLocY;
		getWorld()->determineNext(adjLocX, adjLocY, getDirection()); //find location adjacent to next location, in current direction

		std::list<Actor*> occs = getWorld()->getOccupants(newLocX, newLocY);
		std::list<Actor*>::iterator it = occs.begin();
		Actor* actp;
		while (it != occs.end()) //loop through actors in pending spot
		{
			actp = *it; //derefence iterator
			if (actp != nullptr) //can cast if not null
			{
				Boulder* bp = dynamic_cast<Boulder*>(actp); //check for Boulder
				if (bp != nullptr) //is a boulder
				{
					bp->getsPushedTo(adjLocX, adjLocY); //have boulder determine if movable
				}
			}
			it++; //go to next actor
		}
		if (getWorld()->isWalkableLoc(newLocX, newLocY)) moveTo(newLocX, newLocY);
	}
}

void Player::getsDamaged(int d)
{
	ActorWithHit::getsDamaged(d);
	if (getHitPoints() > 0)
		getWorld()->playSound(SOUND_PLAYER_IMPACT);
	else
		getWorld()->playSound(SOUND_PLAYER_DIE);	
}

bool ActorPickUp::helperDoSomething(int points)
{
	bool withPlayer = false;
	if (!getKeepable()) return false;
	if (getWorld()->isPlayerLoc(getX(),getY())) //same location as player
	{
		withPlayer = true;
		getWorld()->increaseScore(points);
		setKeepable(false); //dead
		getWorld()->playSound(SOUND_GOT_GOODIE); //play sound
	}
	return withPlayer;
}

void Jewel::doSomething()
{
	bool touched = ActorPickUp::helperDoSomething(50);
	if (touched) //on same location as player
	{
		getWorld()->decNumJewels();
	}
}

void Exit::doSomething()
{
	if ((!isActive) && getWorld()->getExitActive())
	{
		isActive = true;
		setVisible(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);

	}
	if(isActive && getWorld()->isPlayerLoc(getX(),getY())) //active, same location as player
	{
		getWorld()->setLevelFin(true);
		getWorld()->increaseScore(2000 + getWorld()->getBonus()); //add to score for finishing level and bonus	
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
	}

	
}

void ELGoodie::doSomething()
{
	bool touched = ActorPickUp::helperDoSomething(1000);
	if (touched)
	{
		getWorld()->incLives();
	}
}

void RHGoodie::doSomething()
{
	bool touched = ActorPickUp::helperDoSomething(500);
	if (touched)
	{
		getWorld()->getPlayer()->setHitPoints(20);
	}
}

void AGoodie::doSomething()
{
	bool touched = ActorPickUp::helperDoSomething(100);
	if (touched)
	{
		getWorld()->getPlayer()->addAmmo(20);
	}
}

void Boulder::getsPushedTo(int x, int y)
{
	if (!(getKeepable())) return;
	std::list<Actor*> occs = getWorld()->getOccupants(x, y);
	std::list<Actor*>::iterator it = occs.begin();
	Actor* actp;
	while (it != occs.end())
	{
		actp = *it;
		if (actp->getBlocksBoulder())
		{
			setBlocksPlayer(true);
			return;
		}
		Hole* hp = dynamic_cast<Hole*>(actp);
		if (hp != nullptr)
		{
			moveTo(x, y);
			return;
		}
		it++;
	}
	setBlocksPlayer(false);
	moveTo(x, y); //either hole or empty space
}

void Hole::doSomething()
{
	if (!getKeepable()) return; //if dead return
	std::list<Actor*> occs = getWorld()->getOccupants(getX(), getY()); //get actors on same locaiton
	std::list<Actor*>::iterator it = occs.begin();
	Actor* actp;
	while (it != occs.end()) //iterate through actors
	{
		actp = *it; //dereference to be actor pointer
		Boulder* bp = dynamic_cast<Boulder*>(actp);
		if (bp != nullptr)
		{
			setKeepable(false);
			bp->setKeepable(false);
			return;
		}
		it++;
	}

}

void Bullet::doSomething()
{
	if (!getKeepable()) return;
	if (affectOccupants()) return; //return if affected occupants
	int newLocX = getX(), newLocY = getY();
	getWorld()->determineNext(newLocX, newLocY, getDirection());
	moveTo(newLocX, newLocY);
	affectOccupants();
}

bool Bullet::affectOccupants()
{
	if (getWorld()->isPlayerLoc(getX(), getY())) //same location as player
	{
		getWorld()->getPlayer()->getsDamaged(2); //damages player 2 points
		setKeepable(false);
		return true;//done acting
	}
	std::list<Actor*> occs = getWorld()->getOccupants(getX(), getY()); //find occupants in same square
	std::list<Actor*>::iterator it = occs.begin();
	Actor* ap; //pointer to an actor
	while (it != occs.end()) //iterate through occupants
	{
		ap = *it;
		if (ap != nullptr)
		{
			ActorWithHit* acthitp = dynamic_cast<ActorWithHit*>(ap);
			if (acthitp != nullptr) //actor has hit points
			{
				acthitp->getsDamaged(2); //gets damaged 2 points and behaves in response
				setKeepable(false); //set self to dead
				return true; //done
			}
			if (ap->getBlocksBullet()) //runs into Wall or Factory
			{
				setKeepable(false); //set self to dead
				return true;
			}

		}
		it++;
	}
	return false;
}

bool Robot::beInactive()
{
	if (!getKeepable()) return true; //if dead return 
	if (!getActive()) //not the active tick yet
	{
		incTick();
		return true;
	}
	return false;
}

bool Robot::playerInPath() //look at location
{
	Direction dir = getDirection();
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getY();
	int itrX = getX(), itrY = getY(); //iterator location first referenced at own loc 
	getWorld()->determineNext(itrX, itrY, dir); //iterator goes to next location

	do
	{
		if (itrX == playerX && itrY == playerY) return true;
		std::list<Actor*> occs = getWorld()->getOccupants(itrX, itrY);
		std::list<Actor*>::iterator occItr = occs.begin();
		Actor* ap;
		while (occItr != occs.end())
		{
			ap = *occItr;
			if (ap->getBlocksShot()) return false; //checks if blocks robot's shot
			occItr++;
		}
		getWorld()->determineNext(itrX, itrY, dir);
	} while (getWorld()->isInGrid(itrX, itrY));
	return false;
}

bool Robot::shoot()
{
	Direction dir = getDirection();
	int newLocX = getX(), newLocY = getY();
	getWorld()->determineNext(newLocX, newLocY, getDirection()); //find next location to place bullet or moev
	if (playerInPath()) //shoot if player is in unobstructed path
	{
		getWorld()->addActor(new Bullet(IID_BULLET, newLocX, newLocY, getWorld(), getDirection())); //place bullet in that location
		getWorld()->playSound(SOUND_ENEMY_FIRE); //shots fired noise
		return true;
	}
	return false;
}

bool Robot::containsObstruc(int x, int y)
{
	if (getWorld()->isPlayerLoc(x, y)) return true; //player obstructs space
	std::list<Actor*> occs = getWorld()->getOccupants(x, y);
	std::list<Actor*>::iterator occItr = occs.begin();
	Actor* ap;
	while (occItr != occs.end())
	{
		ap = *occItr;
		if (ap->getBlocksRob()) return true; //checks if blocks robot's move to 
		occItr++;
	}
	return false;
}

void Robot::getsDamaged(int damage, int points)
{
	ActorWithHit::getsDamaged(damage);
	if (getHitPoints() > 0)
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		getWorld()->playSound(SOUND_ROBOT_DIE);
		getWorld()->increaseScore(points);
	}
}

void SnarlBot::doSomething()
{
	if (Robot::beInactive()) return; //return if not active
	if (shoot()) return;
	//didn't shoot, move instead
	Direction dir = getDirection();
	int newLocX = getX(), newLocY = getY();
	getWorld()->determineNext(newLocX, newLocY, getDirection());
	if (!containsObstruc(newLocX,newLocY)) //move if adj location is free
	{
		moveTo(newLocX,newLocY);
	}
	else //direction change
	{
		if (dir==up) setDirection(down);
		if (dir==down) setDirection(up);
		if (dir==left) setDirection(right);
		if (dir==right) setDirection(left);
	}

}

void SnarlBot::getsDamaged(int d)
{
	Robot::getsDamaged(d, 100);
}

bool KleptoBot::willPickUp()
{
	int j = rand() % 10;
	if (j == 0) return true;
	return false;
}

void KleptoBot::setDistToWalk()
{
	int distBeforeTurn = (rand() % 6) + 1;
}

GraphObject::Direction KleptoBot::genRandDir()
{
	int i = rand() % 4;
	switch (i)
	{
	case(0) :
		return GraphObject::up;
		break;
	case(1) :
		return GraphObject::down;
		break;
	case(2) :
		return GraphObject::left;
		break;
	case(3) :
		return GraphObject::right;
		break;
	}
	return GraphObject::up; //should not get here
}

void KleptoBot::doSomething()
{
	if (Robot::beInactive()) return; //return if not active

	if (hasGoodie == not) //doesnt have a goodie, can pick one up
	{
		std::list<Actor*> occs = getWorld()->getOccupants(getX(), getY()); //looking for goodie
		std::list<Actor*>::iterator occItr = occs.begin();
		Actor* ap;
		while (occItr != occs.end())
		{
			ap = *occItr;
			if (ap->getStealable() && willPickUp()) //checks if occupant is Goodie,pickup chance true
			{
				ActorPickUp* checkGood = dynamic_cast<ActorPickUp*>(ap); //cast to pickupable actor
				if (checkGood != nullptr && (checkGood->getGoodieType())!=not && checkGood->getKeepable()) //is an alive goodie
				{
					hasGoodie = checkGood->getGoodieType(); //robot remembers goodie type
					getWorld()->playSound(SOUND_ROBOT_MUNCH);
					checkGood->setKeepable(false);//kill goodie
					return;
				}
			}
			occItr++;
		}
	}
	//has a goodie already or no goodies to pick up
	if (distMoved < distBeforeTurn)
	{
		int newLocX = getX(), newLocY = getY();
		getWorld()->determineNext(newLocX, newLocY, getDirection());
		if (!containsObstruc(newLocX, newLocY)) //move if adj location is free
		{
			moveTo(newLocX, newLocY);
			distMoved++;
			return; //do nothing more 
		}
	}

	//obstruction or walked max dist
	if (distMoved >= distBeforeTurn)
	{
		distMoved = 0;
		setDistToWalk();
		Direction d = genRandDir();
		std::set<Direction> tried; //tried directions
		while (tried.size()!=4)
		{
			tried.insert(d);
			int newLocX = getX(), newLocY = getY();
			getWorld()->determineNext(newLocX, newLocY, d);
			if (!containsObstruc(newLocX, newLocY))
			{
				moveTo(newLocX, newLocY);
				setDirection(d);
				distMoved++;
				return; //do nothing more 
			}
			d = genRandDir(); //obstructed, find another direction
		} //all directions have obstructions
		setDirection(d);
	}
}

void KleptoBot::getsDamaged(int d)
{
	Robot::getsDamaged(d, 10);
	if (hasGoodie == EL){
		getWorld()->addActor(new ELGoodie(IID_EXTRA_LIFE, getX(), getY(), getWorld()));
	}
	else if (hasGoodie == RH){
		getWorld()->addActor(new RHGoodie(IID_RESTORE_HEALTH, getX(), getY(), getWorld()));
	}
	else if (hasGoodie == A){
		getWorld()->addActor(new AGoodie(IID_AMMO, getX(), getY(), getWorld()));
	}
	
}

bool Factory::willMakeKlept()
{
	int i = rand() % 50;
	if (i == 0)return true;
	return false;
}

void Factory::doSomething()
{
	int x = getX(), y = getY();
	int countKlept = 0;
	bool kleptOnFac = false;
	for (int i = x - 3; i < x + 4; i++)
	{
		for (int j = y - 3; j < y + 4; j++) //loop through surrounding 3x3 square
		{
			if (getWorld()->isInGrid(i, j)) //if valid location
			{
				std::list<Actor*> occs = getWorld()->getOccupants(x, y);
				std::list<Actor*>::iterator occItr = occs.begin();
				Actor* ap;
				while (occItr != occs.end())
				{
					ap = *occItr;
					KleptoBot* kp = dynamic_cast<KleptoBot*>(ap);
					if (kp != nullptr) //if klepto
					{
						if (i == x &&j == y) return; 
						countKlept++;
					}
					occItr++;
				}
			}
		}
	}
	//checked surrounding area
	if (countKlept < 3 && willMakeKlept()) //make a robot
	{	
		if (makeReg) getWorld()->addActor(new KleptoBot(IID_KLEPTOBOT, getX(), getY(), getWorld()));
		else getWorld()->addActor(new AngryKlepto(IID_ANGRY_KLEPTOBOT, getX(), getY(), getWorld())); 
		getWorld()->playSound(SOUND_ROBOT_BORN);
	}
}

void AngryKlepto::doSomething()
{
	if (Robot::beInactive()) return; //return if not active
	if (shoot()) return;
	KleptoBot::doSomething();
}

void AngryKlepto::getsDamaged(int d)
{
	KleptoBot::getsDamaged(d);
	getWorld()->increaseScore(10);
}
