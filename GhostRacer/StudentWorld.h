#ifndef STUDENTWORLD_INCLUDED
#define STUDENTWORLD_INCLUDED

#include "GameWorld.h"
#include <string>
#include <list>
class Actor;
class GhostRacer;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getGhostRacer() const;
    void addActor(Actor* a);
    void recordSoulSaved();
    int getSoulsLeft();
    bool overlaps(const Actor* a1, const Actor* a2) const;
    bool getOverlappingGhostRacer(const Actor* a) const;
    bool sprayOverlap(const Actor *spray);
    double checkCollision(const Actor *actor);
    void createZombieCab();
    void decrementBonus();
    int getBonus();
private:
    GhostRacer* m_ghostracer;
    std::list<Actor*> m_actors;
    int m_souls2save;
    int m_lastWhiteY;
    int m_bonus;
};
#endif // STUDENTWORLD_INCLUDED
