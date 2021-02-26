#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, double x, double y, int dir, double size, unsigned int depth);
    virtual void doSomething() = 0;
    bool isDead() const;
    void setDead();
    StudentWorld* world() const;
    double getVerticalSpeed() const;
    void setVerticalSpeed(double speed);
      // If this actor is affected by holy water projectiles, then inflict that
      // affect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate();
      // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const;
      // Adjust the x coordinate by dx to move to a position with a y coordinate
      // determined by this actor's vertical speed relative to GhostRacser's
      // vertical speed.  Return true if the new position is within the view;
      // otherwise, return false, with the actor dead.
    virtual bool moveRelativeToGhostRacerVerticalSpeed(double dx);
    virtual ~Actor();
private:
    bool m_dead;
    StudentWorld* m_world;
    double m_vspeed;
};

class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, double x, double y, bool isYellow);
    virtual void doSomething();
    virtual bool isCollisionAvoidanceWorthy() const;
    virtual ~BorderLine();
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int hp);
    virtual bool isCollisionAvoidanceWorthy() const;
    int getHP() const;
    void setHP(int hp);
      // Do what the spec says happens when hp units of damage is inflicted.
      // Return true if this agent dies as a result, otherwise false.
    virtual bool takeDamageAndPossiblyDie(int hp);
    virtual int soundWhenHurt();
    virtual int soundWhenDie();
    virtual ~Agent();
private:
    int m_hp;
};

class GhostRacer : public Agent
{
public:
    GhostRacer(StudentWorld* sw, double x, double y);
    void drive();
    virtual void doSomething();
    virtual int soundWhenDie() const;
    int getNumSprays() const;
    void increaseSprays(int amt);
    void spin();
    virtual ~GhostRacer();
private:
    int m_sprays;
};

class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
    int getHorizSpeed() const;
    void setHorizSpeed(int s);
      // Move the pedestrian.  If the pedestrian doesn't go off screen and
      // should pick a new movement plan, pick a new plan.
    void moveAndPossiblyPickPlan();
    virtual ~Pedestrian();
private:
    int m_hspeed;
    
};

class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
    virtual bool takeDamageAndPossiblyDie(int hp);
    virtual ~HumanPedestrian();
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
    virtual ~ZombiePedestrian();
    int getGrunts();
    bool decrementGrunts();
private:
    int m_grunts;
};

class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual int getHSpeed();
    virtual void setHSpeed(int hspeed);
    virtual bool beSprayedIfAppropriate();
    virtual ~ZombieCab();
private:
    int m_hspeed;
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual void doSomething();
    virtual bool isCollisionAvoidanceWorthy() const;
    virtual ~Spray();
    int getPixels();
    void decrementPixels(int pixels); 
private:
    int m_pixels;
};

class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int depth);
    virtual bool beSprayedIfAppropriate();
      // Do the object's special activity (increase health, spin Ghostracer, etc.)
    virtual void doActivity(GhostRacer* gr) = 0;
      // Return the object's increase to the score when activated.
    virtual int getScoreIncrease() const = 0;
    virtual int getSound() const;
      // Return whether the object dies after activation.
    virtual bool selfDestructs() const = 0;
    virtual bool isCollisionAvoidanceWorthy() const;
    virtual bool isSprayable() const = 0;
    virtual void move();
    virtual ~GhostRacerActivatedObject();
};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual int getSound() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
    virtual ~OilSlick();
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
    virtual ~HealingGoodie();
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
    virtual ~HolyWaterGoodie();
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual int getSound() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
    virtual ~SoulGoodie();
};

#endif // ACTOR_INCLUDED
