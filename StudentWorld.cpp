#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_peach = nullptr;
    m_finishedGame = false;
    m_finishedLevel = false;
}


string StudentWorld::levelName(int n)
{
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << n << ".txt";
    return oss.str();
}


void StudentWorld::createGoodie(int startX, int startY, int type) {
    if (type <= 0 && type > 3)
        return;
    Actor* a = nullptr;
    switch (type)
    {
    case 1:
        a = new Star(startX, startY, this, 100);
        break;
    case 2:
        a = new Flower(startX, startY, this, 50);
        break;
    case 3:
        a = new Mushroom(startX, startY, this, 75);
        break;
    }
    m_actors.push_back(a);
}

bool StudentWorld::isBlockingObjectAt(int x, int y) const
{
    for (size_t i = 0; i < m_actors.size(); i++)
        if (m_actors[i]->isAt(x, y) && m_actors[i]->canBlock())
            return true;
    return false;
}

void StudentWorld::bonkAllBonkable(const Actor& bonker, int x, int y)
{
    if (bonker.canBonkPeach() && m_peach->isAt(x, y) && m_peach->isAlive())
        m_peach->bonk();
    else 
        for (size_t k = 0; k < m_actors.size(); k++)
            if (m_actors[k]->isAt(x, y) && m_actors[k]->isAlive())
                m_actors[k]->bonk();
}

bool StudentWorld::damageAllDamageable(const Fireball& damager, int x, int y)
{
    if (damager.canDamagePeach())
    {
        if (m_peach->isAt(x, y) && m_peach->isAlive())
        {
            m_peach->damage();
            return true;
        }
    }
    else
        for (size_t k = 0; k < m_actors.size(); k++)
            if (m_actors[k]->isAt(x, y) && m_actors[k]->canBeDamaged() && m_actors[k]->isAlive())
            {
                m_actors[k]->damage();
                return true;
            }
    return false;
}

void StudentWorld::finishLevel(const EndLevelActors& A)
{
    if (A.canFinishGame())
        m_finishedGame = true;
    else m_finishedLevel = true;
}

int StudentWorld::init()
{
    m_finishedGame = false;
    m_finishedLevel = false;
    Level lev(assetPath());
    string level_file = levelName(getLevel());
    Level::LoadResult result = lev.loadLevel(level_file);
    if (result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        Level::GridEntry ge;
        int x, y;
        for (int lx = 0; lx < GRID_WIDTH; lx++)
            for (int ly = 0; ly < GRID_HEIGHT; ly++)
            {
                ge = lev.getContentsOf(lx, ly);
                x = lx * SPRITE_WIDTH;
                y = ly * SPRITE_HEIGHT;
                switch (ge)
                {
                case Level::empty:
                    break;
                case Level::piranha:
                    m_actors.push_back(new Piranha(x, y, this));
                    break;
                case Level::koopa:
                    m_actors.push_back(new Koopa(x, y, this));
                    break;
                case Level::goomba:
                    m_actors.push_back(new Goomba(x, y, this));
                    break;
                case Level::mario:
                    m_actors.push_back(new Mario(x, y, this));
                    break;
                case Level::flag:
                    m_actors.push_back(new Flag(x, y, this));
                    break;
                case Level::pipe:
                    m_actors.push_back(new Pipe(x, y, this));
                    break;
                case Level::block:
                    m_actors.push_back(new Block(x, y, this, 0));
                    break;
                case Level::star_goodie_block:
                    m_actors.push_back(new Block(x, y, this, 1));
                    break;
                case Level::flower_goodie_block:
                    m_actors.push_back(new Block(x, y, this, 2));
                    break;
                case Level::mushroom_goodie_block:
                    m_actors.push_back(new Block(x, y, this, 3));
                    break;
                case Level::peach:
                    m_peach = new Peach(x, y, this);
                    break;
                }
            }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (m_actors[k]->isAlive())
        {
            m_actors[k]->doSomething();
            if (!(m_peach->isAlive()))
            {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_finishedLevel)
            {
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
            if (m_finishedGame)
            {
                playSound(SOUND_GAME_OVER);
                return GWSTATUS_PLAYER_WON;
            }
        }
    }
    m_peach->doSomething();
    if (!m_peach->isAlive())
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    vector<Actor*>::iterator it = m_actors.begin();

    while (it != m_actors.end())
        if (!(*it)->isAlive())
        {
            delete* it;
            it = m_actors.erase(it);
        }
        else it++;
    ostringstream oss;
    oss.fill('0');
    oss << "Lives: " << getLives() << "  Level: " << setw(2) << getLevel() << "  Points: " << setw(6) << getScore();
    if (m_peach->hasStarP())
        oss << " StarPower!";
    if (m_peach->hasShootP())
        oss << " ShootPower!";
    if (m_peach->hasJumpP())
        oss << " JumpPower!";
    string s = oss.str();
    setGameStatText(s);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_peach != nullptr) 
        delete m_peach;
    vector<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        delete* it;
        it = m_actors.erase(it);
    }
}


