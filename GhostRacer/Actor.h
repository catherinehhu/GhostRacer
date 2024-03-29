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
    virtual bool beSprayedIfAppropriate();
    virtual bool isCollisionAvoidanceWorthy() const;
    virtual void getSprayed();
    virtual ~Actor();
private:
    bool m_dead;
    double m_vspeed;
    StudentWorld* m_world;
};
class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, double x, double y, bool isYellow);
    virtual void doSomething();
    virtual ~BorderLine();
};
class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int hp);
    virtual bool isCollisionAvoidanceWorthy() const;
    int getHP() const;
    void setHP(int hp);
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
    virtual void moveAndPossiblyPickPlan();
    void setPlan(int plan);
    virtual bool beSprayedIfAppropriate();
    void decrementPlan();
    void move();
    int getPlan();
    virtual ~Pedestrian();
private:
    int m_hspeed;
    int m_plan;
};
class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void getSprayed();
    virtual ~HumanPedestrian();
};
class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual ~ZombiePedestrian();
    virtual void getSprayed();
    int getGrunts();
    void decrementGrunts();
private:
    int m_grunts;
};
class ZombieCab : public Pedestrian
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    bool checkDamage();
    void doneDamage();
    virtual void getSprayed();
    virtual void moveAndPossiblyPickPlan();
    virtual ~ZombieCab();
private:
    bool m_damage;
};
class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual void doSomething();
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
    virtual void doActivity(GhostRacer* gr) = 0;
    virtual int getScoreIncrease() const = 0;
    virtual int getSound() const;
    virtual bool selfDestructs() const = 0;
    virtual void getSprayed();
    virtual void doSomething();
    void move();
    virtual ~GhostRacerActivatedObject();
};
class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual int getSound() const;
    virtual bool selfDestructs() const;
    virtual ~OilSlick();
};
class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool beSprayedIfAppropriate();
    virtual bool selfDestructs() const;
    virtual ~HealingGoodie();
};
class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool selfDestructs() const;
    virtual bool beSprayedIfAppropriate(); 
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
    virtual ~SoulGoodie();
};

#endif // ACTOR_INCLUDED
