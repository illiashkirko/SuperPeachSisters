#include "Actor.h"
#include "StudentWorld.h"

/////////////////////////////////////////
//PEACH FUNCTIONS IMPLEMENTATIONS BEGIN//
/////////////////////////////////////////

void Peach::doSomething()
{
	if (!isAlive())
		return;
	if (m_timeInvincible > 0)
		m_timeInvincible--;
	if (m_timeInvincible == 0)
		m_star = false;
	if (m_timeTempInvincible > 0)
		m_timeTempInvincible--;
	if (m_timeRecharge > 0)
		m_timeRecharge--;
	studentWorldPtr()->bonkAllBonkable(*this, getX(), getY());
	if (remaining_jump_distance > 0)
	{
		if (studentWorldPtr()->isBlockingObjectAt(getX(), getY() + 4))
		{
			studentWorldPtr()->bonkAllBonkable(*this, getX(), getY() + 4);
			remaining_jump_distance = 0;
		}
		else
		{
			moveTo(getX(), getY() + 4);
			remaining_jump_distance--;
		}
	}
	else
		if (!studentWorldPtr()->isBlockingObjectAt(getX(), getY() - 3))
			moveTo(getX(), getY() - 4);

	int key = -1;
	if (studentWorldPtr()->getKey(key))
	{
		int targetX = getX(), targetY = getY();
		switch (key)
		{
		case KEY_PRESS_LEFT:
			setDirection(180);
			targetX -= 4;
			break;
		case KEY_PRESS_RIGHT:
			setDirection(0);
			targetX += 4;
			break;
		}
		if (targetX != getX() && studentWorldPtr()->isBlockingObjectAt(targetX, targetY))
			studentWorldPtr()->bonkAllBonkable(*this, targetX, targetY);
		else if (targetX != getX())
			moveTo(targetX, targetY);

		switch (key)
		{
		case KEY_PRESS_UP:
			if (studentWorldPtr()->isBlockingObjectAt(getX(), getY() - 1))
			{
				remaining_jump_distance = m_jump ? 12 : 8;
				studentWorldPtr()->playSound(SOUND_PLAYER_JUMP);
			}
			break;
		case KEY_PRESS_SPACE:
			if (m_shoot && m_timeRecharge == 0)
			{
				studentWorldPtr()->playSound(SOUND_PLAYER_FIRE);
				m_timeRecharge = 8;
				int fireX = getX() + (getDirection() == 0 ? 4 : -4);
				int fireY = getY();
				studentWorldPtr()->createPeachFireball(fireX, fireY, getDirection());
			}
			break;
		}
	}
}

void Peach::bonk()
{
	if (m_timeInvincible > 0 || m_timeTempInvincible > 0)
		return;
	m_hitpoints--;
	m_timeTempInvincible = 10;
	m_shoot = false;
	m_jump = false;
	if (m_hitpoints == 1)
		studentWorldPtr()->playSound(SOUND_PLAYER_HURT);
	if (m_hitpoints <= 0)
		setDead();
}


///////////////////////////////////////
//PEACH FUNCTIONS IMPLEMENTATIONS END//
///////////////////////////////////////


//////////////////////////////////////////
//BLOCKS FUNCTIONS IMPLEMENTATIONS BEGIN//
//////////////////////////////////////////

void Block::bonk()
{
	if (m_goodie == 0)
	{
		studentWorldPtr()->playSound(SOUND_PLAYER_BONK);
		return;
	}
	studentWorldPtr()->playSound(SOUND_POWERUP_APPEARS);
	studentWorldPtr()->createGoodie(getX(), getY() + 8, m_goodie);
	m_goodie = 0;
}

////////////////////////////////////////
//BLOCKS FUNCTIONS IMPLEMENTATIONS END//
////////////////////////////////////////


//////////////////////////////////////////////////
//ENDLEVELACTORS FUNCTIONS IMPLEMENTATIONS BEGIN//
//////////////////////////////////////////////////

void EndLevelActors::doSomething()
{
	if (!isAlive())
		return;
	if (studentWorldPtr()->isOverlapWithPeach(getX(), getY()))
	{
		studentWorldPtr()->increaseScore(1000);
		setDead();
		studentWorldPtr()->finishLevel(*this);
	}
}

////////////////////////////////////////////////
//ENDLEVELACTORS FUNCTIONS IMPLEMENTATIONS END//
////////////////////////////////////////////////


///////////////////////////////////////////
//GOODIES FUNCTIONS IMPLEMENTATIONS BEGIN//
///////////////////////////////////////////
void Helper::doSomething()
{
	if (!isAlive())
		return;
	if (doHelperStuff())
		return;
	if (!studentWorldPtr()->isBlockingObjectAt(getX(), getY() - 2))
		moveTo(getX(), getY() - 2);
	int targetX = getX();
	if (getDirection() == 0)
		targetX += 2;
	if (getDirection() == 180)
		targetX -= 2;
	if (studentWorldPtr()->isBlockingObjectAt(targetX, getY()))
		doHelperWhenNoMove();
	else
		moveTo(targetX, getY());
}

void Goodie::doHelperWhenNoMove()
{
	if (getDirection() == 0)
		setDirection(180);
	else
		setDirection(0);
}

void Fireball::doHelperWhenNoMove()
{
	setDead();
}

bool Goodie::doHelperStuff()
{
	if (studentWorldPtr()->isOverlapWithPeach(getX(), getY()))
	{
		studentWorldPtr()->increaseScore(getScore());
		doGoodieStuff();
		studentWorldPtr()->increaseHitPoints(2);
		setDead();
		studentWorldPtr()->playSound(SOUND_PLAYER_POWERUP);
		return true;
	}
	return false;
}

bool Fireball::doHelperStuff()
{
	if (studentWorldPtr()->damageAllDamageable(*this, getX(), getY()))
	{
		setDead();
		return true;
	}
	return false;
}

void Flower::doGoodieStuff()
{
	studentWorldPtr()->setShootPower();
}
void Mushroom::doGoodieStuff()
{
	studentWorldPtr()->setJumpPower();
}
void Star::doGoodieStuff()
{
	studentWorldPtr()->setInvincibility(150);
}

/////////////////////////////////////////
//HELPRES FUNCTIONS IMPLEMENTATIONS END//
/////////////////////////////////////////


///////////////////////////////////////////
//ENEMIES FUNCTIONS IMPLEMENTATIONS BEGIN//
///////////////////////////////////////////	
void Enemy::doSomething()
{
	if (!isAlive())
		return;
	if (attack())
		return;
	switch (getDirection())
	{
	case 0:
		if (studentWorldPtr()->isBlockingObjectAt(getX() + 1, getY()))
			setDirection(180);
		else if (!studentWorldPtr()->isBlockingObjectAt(getX() + 8, getY() - 1))
			setDirection(180);
		break;
	case 180:
		if (studentWorldPtr()->isBlockingObjectAt(getX() - 1, getY()))
			setDirection(0);
		else if (!studentWorldPtr()->isBlockingObjectAt(getX() - 8, getY() - 1))
			setDirection(0);
		break;
	}
	int targetX = getX();
	if (getDirection() == 0) targetX++;
	else targetX--;
	if (studentWorldPtr()->isBlockingObjectAt(targetX, getY()))
		return;
	moveTo(targetX, getY());
}

bool Enemy::attack()
{
	if (studentWorldPtr()->isOverlapWithPeach(getX(), getY()))
	{
		studentWorldPtr()->bonkAllBonkable(*this, getX(), getY());
		return true;
	}
	return false;
}
void Enemy::bonk()
{
	if (studentWorldPtr()->peachHasStarP())
	{
		studentWorldPtr()->playSound(SOUND_PLAYER_KICK);
		damage();
	}
}
void Enemy::damage()
{
	studentWorldPtr()->increaseScore(100);
	setDead();
	doEnemiesStuff();
}

void Koopa::doEnemiesStuff()
{
	studentWorldPtr()->createShell(getX(), getY(), getDirection());
}

void Piranha::doSomething()
{
	if (!isAlive())
		return;
	increaseAnimationNumber();
	if (attack())
		return;
	double peachX = studentWorldPtr()->getPeachXCoord();
	double peachY = studentWorldPtr()->getPeachYCoord();
	if (!(peachY - getY() <= 1.5 * SPRITE_HEIGHT && peachY - getY() >= -1.5 * SPRITE_HEIGHT))
		return;
	if (getX() > peachX)
		setDirection(180);
	else setDirection(0);
	if (firing_delay > 0)
	{
		firing_delay--;
		return;
	}
	if (getX() - peachX < 8.0 * SPRITE_WIDTH && getX() - peachX > - 8.0 * SPRITE_WIDTH)
	{
		studentWorldPtr()->createPiranhaFireball(getX(), getY(), getDirection());
		studentWorldPtr()->playSound(SOUND_PIRANHA_FIRE);
		firing_delay = 40;
	}
}
/////////////////////////////////////////
//ENEMIES FUNCTIONS IMPLEMENTATIONS END//
/////////////////////////////////////////
