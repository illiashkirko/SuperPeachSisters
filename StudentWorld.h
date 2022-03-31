#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <vector>
#include <string>

using namespace std;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld() { cleanUp(); }
	virtual int init();
	virtual int move();
	virtual void cleanUp();
//SETTERS
	void setShootPower() { m_peach->setShootPower(); }
	void setJumpPower() { m_peach->setJumpPower(); }
	void setInvincibility(int t) { m_peach->setInvincibility(t); }
	void increaseHitPoints(int n) { m_peach->increaseHitPoints(n); }
//GETTERS
	bool isBlockingObjectAt(int x, int y) const;
	bool isOverlapWithPeach(int x, int y) const { return m_peach->isAt(x, y); }
	bool peachHasStarP() const { return m_peach->hasStarP(); }
	double getPeachXCoord() const { return m_peach->getX(); }
	double getPeachYCoord() const { return m_peach->getY(); }
//CREATING ACTORS
	void createPiranhaFireball(int startX, int startY, int dir) {
		m_actors.push_back(new PiranhaFireball(startX, startY, this, dir));
	}
	void createPeachFireball(int startX, int startY, int dir) {
		m_actors.push_back(new PeachFireball(startX, startY, this, dir));
	}
	void createShell(int startX, int startY, int dir) {
		m_actors.push_back(new Shell(startX, startY, this, dir));
	}
	void createGoodie(int startX, int startY, int type);
//HELPER MEMBER FUNCTIONS
	string levelName(int n);
//OTHER MEMBER FUNCTIONS
	void bonkAllBonkable(const Actor& bonker, int x, int y);
	bool damageAllDamageable(const Fireball& damager, int x, int y);
	void finishLevel(const EndLevelActors& A);
	
private:
	vector< Actor* > m_actors;
	Peach* m_peach;
	bool m_finishedLevel;
	bool m_finishedGame;
};

#endif // STUDENTWORLD_H_
