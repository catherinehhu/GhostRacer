// A possible interface for StudentWorld.  You may use all, some, or none
// of this, as you wish.

// One of the design goals is to reduce the coupling between StudentWorld.h
// and Actor.h as much as possible.  Notice that in fact, StudentWorld.h
// does not need to include Actor.h at all, and knows only two names in the
// Actor hierarchy.

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
      // If actor a overlaps this world's GhostRacer, return a pointer to the
      // GhostRacer; otherwise, return nullptr
    bool getOverlappingGhostRacer(Actor* a) const;
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
