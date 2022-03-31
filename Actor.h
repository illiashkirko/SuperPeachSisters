#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* studWorld, bool canBlock, bool canBeDamaged,
		bool canBonkPeach, int dir = 0, int depth = 0, double size = 1.0)
		:GraphObject(imageID, startX, startY, dir, depth, size) {
		m_world = studWorld;
		can_block = canBlock;
		is_alive = true;
		can_be_damaged = canBeDamaged;
		can_bonk_peach = canBonkPeach;
	}
//SETTERS
	void setDead() { is_alive = false; }
//GETTERS
	bool isAlive() const { return is_alive; }
	bool canBlock() const { return can_block; }
	bool isAt(int x, int y) const { 
		return abs(getX() - x) < SPRITE_WIDTH && abs(getY() - y) < SPRITE_HEIGHT;
	}
	bool canBeDamaged() const { return can_be_damaged; }
	bool canBonkPeach() const { return can_bonk_peach; }
	StudentWorld* studentWorldPtr() const { return m_world; }
//OTHER MEMBER FUNCTIONS
	virtual void doSomething() = 0;
	virtual void bonk() {};
	virtual void damage() {};

private:
	bool is_alive;
	StudentWorld* m_world;
	bool can_block;
	bool can_be_damaged;
	bool can_bonk_peach;
};

/////////////////////////////////
//PEACH CLASS DEFINITIONS BEGIN//
/////////////////////////////////

class Peach : public Actor
{
public:
	Peach(int startX, int startY, StudentWorld* studWorld)
		:Actor(IID_PEACH, startX, startY, studWorld, false, true, false, 0, 0, 1.0), m_timeInvincible(0), m_timeTempInvincible(0),
		m_timeRecharge(0), m_star(false), m_jump(false), m_shoot(false),
		remaining_jump_distance(0), m_hitpoints(1) {}
//SETTERS
	void setShootPower() { m_shoot = true; }
	void setJumpPower() { m_jump = true; }
	void setInvincibility(int t) { m_star = true; m_timeInvincible = t; }
	void setJumpDistance(int d) { remaining_jump_distance = d; }
	void increaseHitPoints(int n) { m_hitpoints += n; }
//GETTERS
	bool hasStarP() const { return m_star; }
	bool hasJumpP() const { return m_jump; }
	bool hasShootP() const { return m_shoot; }
//OTHER MEMBER FUNCTIONS
	virtual void doSomething();
	virtual void bonk();
	virtual void damage() { bonk(); }

private:
	int m_timeInvincible, m_timeTempInvincible;
	int m_timeRecharge;
	bool m_star, m_jump, m_shoot;
	int remaining_jump_distance;
	int m_hitpoints;
};

///////////////////////////////
//PEACH CLASS DEFINITIONS END//
///////////////////////////////


/////////////////////////////////
//WALLS CLASS DEFINITIONS BEGIN//
/////////////////////////////////

class Walls : public Actor
{
public:
	Walls(int IID, int startX, int startY, StudentWorld* studWorld)
		:Actor(IID, startX, startY, studWorld, true, false, false, 0, 2, 1.0) {}
	virtual void doSomething() {}
};

class Pipe : public Walls
{
public:
	Pipe(int startX, int startY, StudentWorld* studWorld)
		:Walls(IID_PIPE, startX, startY, studWorld) {}
};

class Block : public Walls
{
public:
	Block(int startX, int startY, StudentWorld* studWorld, int goodieType)
		:Walls(IID_BLOCK, startX, startY, studWorld) {
		m_goodie = goodieType;
	}
	virtual void bonk();
private:
	int m_goodie;
};

///////////////////////////////
//WALLS CLASS DEFINITIONS END//
///////////////////////////////


//////////////////////////////////////////
//ENDLEVELACTORS CLASS DEFINITIONS BEGIN//
//////////////////////////////////////////

class EndLevelActors : public Actor
{
public:
	EndLevelActors(int IID, int startX, int startY, StudentWorld* studWorld, bool can_finish)
		:Actor(IID, startX, startY, studWorld, false, false, false, 0, 1, 1), can_finish_game(can_finish) {}
	bool canFinishGame() const { return can_finish_game; }
	virtual void doSomething();

private:
	bool can_finish_game;
};

class Flag : public EndLevelActors
{
public:
	Flag(int startX, int startY, StudentWorld* studWorld)
		:EndLevelActors(IID_FLAG, startX, startY, studWorld, false) {}
};

class Mario : public EndLevelActors
{
public:
	Mario(int startX, int startY, StudentWorld* studWorld)
		:EndLevelActors(IID_MARIO, startX, startY, studWorld, true) {}
};

////////////////////////////////////////
//ENDLEVELACTORS CLASS DEFINITIONS END//
////////////////////////////////////////

/////////////////////////////////////////////
//GOODIES+FIREBALLS CLASS DEFINITIONS BEGIN//
/////////////////////////////////////////////

class Helper : public Actor
{
public:
	Helper(int IID, int startX, int startY, StudentWorld* studWorld, int dir)
		:Actor(IID, startX, startY, studWorld, false, false, false, dir, 1, 1) {}
	virtual void doSomething();

private:
	virtual void doHelperWhenNoMove() = 0;
	virtual bool doHelperStuff() = 0;
};

///////////////////////////////////
//HELPERS CLASS DEFINITIONS BEGIN//
///////////////////////////////////

//GOODIE

class Goodie : public Helper
{
public:
	Goodie(int IID, int startX, int startY, StudentWorld* studWorld, int score)
		:Helper(IID, startX, startY, studWorld, 0), m_score(score) {}
	int getScore() { return m_score; }

private:
	virtual void doGoodieStuff() = 0;
	virtual bool doHelperStuff();
	virtual void doHelperWhenNoMove();
	int m_score;
};

class Flower : public Goodie
{
public:
	Flower(int startX, int startY, StudentWorld* studWorld, int score)
		:Goodie(IID_FLOWER, startX, startY, studWorld, score) {}

private:
	virtual void doGoodieStuff();
};

class Mushroom : public Goodie
{
public:
	Mushroom(int startX, int startY, StudentWorld* studWorld, int score)
		:Goodie(IID_MUSHROOM, startX, startY, studWorld, score) {}

private:
	virtual void doGoodieStuff();
};

class Star : public Goodie
{
public:
	Star(int startX, int startY, StudentWorld* studWorld, int score)
		:Goodie(IID_STAR, startX, startY, studWorld, score) {}

private:
	virtual void doGoodieStuff();
};

//FIREBALLS

class Fireball : public Helper
{
public:
	Fireball(int IID, int startX, int startY, StudentWorld* studWorld, int dir)
		:Helper(IID, startX, startY, studWorld, dir), can_damage_peach(false) {}
	void setCanDamagePeach(bool x) { can_damage_peach = x; }
	bool canDamagePeach() const { return can_damage_peach; }

private:
	virtual void doHelperWhenNoMove();
	virtual bool doHelperStuff();
	bool can_damage_peach;
};

class PiranhaFireball : public Fireball
{
public:
	PiranhaFireball(int startX, int startY, StudentWorld* studWorld, int dir)
		:Fireball(IID_PIRANHA_FIRE, startX, startY, studWorld, dir) {
		setCanDamagePeach(true);
	}
};

class PeachFireball : public Fireball
{
public:
	PeachFireball(int startX, int startY, StudentWorld* studWorld, int dir)
		:Fireball(IID_PEACH_FIRE, startX, startY, studWorld, dir) {}
};

class Shell : public Fireball
{
public:
	Shell(int startX, int startY, StudentWorld* studWorld, int dir)
		:Fireball(IID_SHELL, startX, startY, studWorld, dir) {}
};

///////////////////////////////////////////
//GOODIES+FIREBALLS CLASS DEFINITIONS END//
///////////////////////////////////////////


///////////////////////////////////
//ENEMIES CLASS DEFINITIONS BEGIN//
///////////////////////////////////

class Enemy : public Actor
{
public:
	Enemy(int IID, int startX, int startY, StudentWorld* studWorld)
		:Actor(IID, startX, startY, studWorld, false, true, true, randInt(0, 1) * 180, 1, 0) {}
	virtual void doSomething();
	bool attack();

private:
	virtual void doEnemiesStuff() {}
	virtual void bonk();
	virtual void damage();
};	

class Piranha : public Enemy
{
public:
	Piranha(int startX, int startY, StudentWorld* studWorld)
		:Enemy(IID_PIRANHA, startX, startY, studWorld), firing_delay(0) {}
	virtual void doSomething();

private:
	int firing_delay;
};

class Goomba : public Enemy
{
public:
	Goomba(int startX, int startY, StudentWorld* studWorld)
		:Enemy(IID_GOOMBA, startX, startY, studWorld) {}
};

class Koopa : public Enemy
{
public:
	Koopa(int startX, int startY, StudentWorld* studWorld)
		:Enemy(IID_KOOPA, startX, startY, studWorld) {}

private:
	virtual void doEnemiesStuff();
};

/////////////////////////////////
//ENEMIES CLASS DEFINITIONS END//
/////////////////////////////////

#endif // ACTOR_H_
