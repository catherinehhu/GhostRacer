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

      // Return a pointer to the world's GhostRacer.
    GhostRacer* getGhostRacer();

      // Add an actor to the world.
    void addActor(Actor* a);

      // Record that a soul was saved.
    void recordSoulSaved();
    
    int getSoulsSaved(); 
    
    void makeSpray(); 

      // If actor a overlaps some live actor that is affected by a holy water
      // projectile, inflict a holy water spray on that actor and return true;
      // otherwise, return false.  (See Actor::beSprayedIfAppropriate.)
    bool sprayFirstAppropriateActor(Actor* a);

      // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor* a1, const Actor* a2) const;

      // If actor a overlaps this world's GhostRacer, return a pointer to the
      // GhostRacer; otherwise, return nullptr
    bool getOverlappingGhostRacer(Actor* a) const;
private:
    GhostRacer* m_ghostracer;
    std::list<Actor*> m_actors;
    int m_souls;
};

#endif // STUDENTWORLD_INCLUDED
