#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
	enum Goodie { EL, RH, A, not };
	Actor(int imageID, int startX, int startY, StudentWorld* w, Direction dir)
		:GraphObject(imageID, startX, startY, dir), world(w), blocksPlayer(false), blocksBoulder(true), toBeKept(true), 
		blocksBullet(false), blocksShot(false), blocksRob(false), isStealable(false)
	{
		//blocksPlayer is true for Boulders in corners, Wall, Hole, Robot, Factory
		//toBeKept is true Wall, Factories, Exit
		setVisible(true); //EXCEPT FOR EXIT
	}
	virtual ~Actor()
	{
	}
	virtual void doSomething() = 0;
	
	StudentWorld* getWorld()
	{
		return world;
	}
	
	void setBlocksPlayer(bool val)
	{
		blocksPlayer = val;
	}
	bool getBlocksPlayer()
	{
		return blocksPlayer;
	}
	void setBlocksBoulder(bool val)
	{
		blocksBoulder = val;
	}
	bool getBlocksBoulder()
	{
		return blocksBoulder;
	}
	virtual void setKeepable(bool val)
	{	
		toBeKept = val;
	}
	virtual bool getKeepable()
	{
		return toBeKept;
	}
	bool getBlocksBullet()
	{
		return blocksBullet;
	}
	void setBlocksBullet(bool val)
	{
		blocksBullet = val;
	}
	bool getBlocksShot()
	{
		return blocksShot;
	}
	void setBlocksShot(bool val)
	{
		blocksShot = val;
	}
	bool getBlocksRob()
	{
		return blocksRob;
	}
	void setBlocksRob(bool val)
	{
		blocksRob = val;
	}
	bool getStealable()
	{
		return isStealable;
	}
	void setStealable(bool val)
	{
		isStealable = val;
	}
private:
	StudentWorld* world;
	bool blocksPlayer, blocksBoulder, toBeKept, blocksBullet, blocksShot, blocksRob, isStealable; 
};

class ActorWithHit : public Actor
{
public:
	ActorWithHit(int imageID, int startX, int startY, StudentWorld* w, Direction dir)
		:Actor(imageID, startX, startY, w, dir), health(0)
	{
		setBlocksRob(true);
	}
	virtual ~ActorWithHit()
	{
	}
	virtual void doSomething() = 0;
	int getHitPoints()
	{
		return health;
	}
	void setHitPoints(int h)
	{
		health = h;
	}
	virtual void setKeepable(bool val)
	{
		if (!val) //false
		{
			setHitPoints(0);
			return;
		}
		setHitPoints(10); //if true, full health?
	}
	virtual bool getKeepable()
	{
		return (getHitPoints()>0);
	}
	virtual void getsDamaged(int d)
	{
		health -= d; //health loses number of points
		if (health < 0) health = 0;
	}
private:
	int health;
};

class Player : public ActorWithHit
{
public:
	Player(int imageID, int startX, int startY, StudentWorld* w, Direction dir = right)
		:ActorWithHit(imageID, startX, startY, w, dir), ammo(20)
	{
		setHitPoints(20);
	}
	virtual ~Player()
	{
	}
	virtual void doSomething();

	int getAmmo()
	{
		return ammo;
	}
	void setAmmo(int val)
	{
		ammo = val;
	}
	void addAmmo(int val)
	{
		ammo += val;
	}
	virtual void getsDamaged(int d);
private:
	int ammo;
};

class Wall : public Actor
{
public:
	Wall(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:Actor(imageID, startX, startY, w, dir)
	{
		setBlocksPlayer(true);
		setBlocksBullet(true);
		setBlocksShot(true);
		setBlocksRob(true);
	}
	virtual ~Wall()
	{
	}
	virtual void doSomething()
	{
	}
};
 
class ActorPickUp : public Actor
{
public:
	ActorPickUp(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:Actor(imageID, startX, startY, w, dir), isGoodie(not)
	{
		setStealable(true);
	}
	virtual ~ActorPickUp()
	{
	}
	virtual void doSomething() = 0;
	Goodie getGoodieType()
	{
		return isGoodie;
	}
	void setGoodieType(Goodie g)
	{
		isGoodie = g;
	}

protected:
	bool helperDoSomething(int points);
	Goodie isGoodie;

};


class Jewel : public ActorPickUp
{
public:
	Jewel(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:ActorPickUp(imageID, startX, startY, w, dir)
	{
		setStealable(false);
	}
	virtual ~Jewel()
	{
	}
	virtual void doSomething();
};

class Exit : public Actor
{
public:
	Exit(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:Actor(imageID, startX, startY, w, dir), isActive(false)
	{
		setVisible(false);
	}
	virtual ~Exit()
	{
	}
	virtual void doSomething();
	
private:
	bool isActive;
};

class ELGoodie : public ActorPickUp
{
public:
	ELGoodie(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:ActorPickUp(imageID, startX, startY, w ,dir)
	{
		setGoodieType(EL);
	}
	virtual ~ELGoodie()
	{
	}
	virtual void doSomething();

};

class RHGoodie : public ActorPickUp
{
public:
	RHGoodie(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:ActorPickUp(imageID, startX, startY, w, dir)
	{
		setGoodieType(RH);
	}
	virtual ~RHGoodie()
	{
	}
	virtual void doSomething();
};

class AGoodie : public ActorPickUp
{
public:
	AGoodie(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:ActorPickUp(imageID, startX, startY, w, dir)
	{
		setGoodieType(A);
	}
	virtual ~AGoodie()
	{
	}
	virtual void doSomething();
};

class Boulder : public ActorWithHit
{
public:
	Boulder(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:ActorWithHit(imageID, startX, startY, w, dir)
	{
		setHitPoints(10); 
		setBlocksShot(true);
		//cannot be pushed against Boulders, Walls, otherActors
	}
	virtual ~Boulder()
	{
	}
	virtual void doSomething() {}
	void getsPushedTo(int x, int y);
};

class Hole : public Actor
{
public:
	Hole(int imageID, int startX, int startY, StudentWorld* w, Direction dir = none)
		:Actor(imageID, startX, startY, w, dir)
	{
		setBlocksPlayer(true);
		setBlocksBoulder(false);
		setBlocksRob(true);
	}
	virtual ~Hole()
	{
	}
	virtual void doSomething(); //Boulder gets pushed on it
};

class Bullet : public Actor
{
public:
	Bullet(int imageID, int startX, int startY, StudentWorld* w, Direction dir)
		:Actor(imageID, startX, startY, w, dir)
	{
		setBlocksBoulder(false);
	}
	virtual ~Bullet()
	{
	}
	virtual void doSomething();
	bool affectOccupants();
	
};

class Robot : public ActorWithHit
{
public:
	Robot(int imageId, int startX, int startY, StudentWorld* w, Direction dir)
		:ActorWithHit(imageId, startX, startY, w, dir), curTick(0)
	{
		setBlocksPlayer(true);
		setBlocksShot(true);
		maxTick = (28 - (getWorld()->getLevel()))/4;
		if (maxTick < 3) maxTick = 3;
	
	}
	virtual ~Robot()
	{
	}
	virtual void doSomething() = 0;
	bool beInactive();
	bool playerInPath();
	bool shoot();
	bool containsObstruc(int x, int y);
	int getActive() //tell if current tick is an active tick
	{
		if (curTick >= maxTick)
		{
			curTick = 0;
			return true;
		}
		return false;
	}
	
	void incTick() //increment tick
	{
		curTick++;

	}
	virtual void getsDamaged(int damage, int points);
private:
	int curTick, maxTick; //rest based on current tick
};

class SnarlBot : public Robot
{
public:
	SnarlBot(int imageId, int startX, int startY, StudentWorld* w, Direction dir)
		:Robot(imageId, startX, startY, w, dir)
	{
		setHitPoints(10);
	}
	virtual ~SnarlBot()
	{
	}
	virtual void doSomething();
	virtual void getsDamaged(int d);
};

class KleptoBot : public Robot
{
public:
	KleptoBot(int imageId, int startX, int startY, StudentWorld* w, Direction dir = right)
		: Robot(imageId, startX, startY, w, dir), hasGoodie(not), distMoved(0)
	{
		setHitPoints(5);
		setDistToWalk();
	}
	virtual ~KleptoBot()
	{
	}
	virtual void doSomething();
	bool willPickUp();
	void setDistToWalk();
	Direction genRandDir();
	Goodie getGoodie()
	{
		return hasGoodie;
	}
	void setGoodie(Goodie g)
	{
		hasGoodie = g;
	}
	virtual void getsDamaged(int d);
private:
	Goodie hasGoodie;
	int distBeforeTurn, distMoved;
};

class AngryKlepto : public KleptoBot
{
public:
	AngryKlepto(int imageId, int startX, int startY, StudentWorld* w, Direction dir = right)
		: KleptoBot(imageId, startX, startY, w, dir)
	{
		setHitPoints(8);
		setDistToWalk();
	}
	virtual ~AngryKlepto()
	{
	}
	virtual void doSomething();
	virtual void getsDamaged(int d);

};

class Factory : public Actor
{
public:
	Factory(int imageId, int startX, int startY, StudentWorld* w, bool isReg, Direction dir = none)
		: Actor(imageId, startX, startY, w, dir), makeReg(isReg)
	{
		setBlocksPlayer(true);
		setBlocksBoulder(true);
		setBlocksBullet(true);
		setBlocksShot(true);
		setBlocksRob(true);
	}
	virtual ~Factory()
	{
	}
	bool willMakeKlept();
	virtual void doSomething();
private:
	bool makeReg;
};


//FACTORY  blocksplayer, blocksboulder, blocksBullet, setBlocksShot(true), setBlocksRob(true);


#endif // ACTOR_H_
