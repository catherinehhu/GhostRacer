#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
const double PI = 3.14159265;
const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* sw, int imageID, double x, double y, int dir = 0, double size = 0, unsigned int depth = 0)
:GraphObject(imageID, x, y, dir, size, depth)
{
    m_dead = false;
    m_world = sw;
    m_vspeed = 0;
}
Actor::~Actor(){
    
}
bool Actor::isDead() const{
    return m_dead;
}

void Actor::setDead(){
    m_dead = true;
}

StudentWorld* Actor::world() const{
    return m_world;
}

double Actor::getVerticalSpeed() const{
    return m_vspeed;
}

void Actor::setVerticalSpeed(double speed){
    m_vspeed = speed;
}

bool Actor::beSprayedIfAppropriate(){
    return false;
}

bool Actor::isCollisionAvoidanceWorthy() const{
    return true;
}

bool Actor::moveRelativeToGhostRacerVerticalSpeed(double dx){
    double vert_speed = getVerticalSpeed() - world() ->getGhostRacer()->getVerticalSpeed();
    double new_y = getY() + vert_speed;
    double new_x = dx;
    moveTo(new_x, new_y);
    if (new_x < 0 || new_y > 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT){
        setDead();
        return false;
    }
    else{
        return true;
    }
}

BorderLine::BorderLine(StudentWorld* sw, double x, double y, bool isYellow)
:Actor(sw, ((isYellow) ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE), x, y, 0, 2.0, 1)
{
    setVerticalSpeed(-4.0);
}
BorderLine::~BorderLine(){}
void BorderLine::doSomething(){
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX();
    moveTo(new_x, new_y);
    if (new_x < 0 || new_y < 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
}
bool BorderLine::isCollisionAvoidanceWorthy() const{
    return false; 
}

Agent::Agent(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int hp)
: Actor(sw, imageID, x, y, dir, size, 0)
{
    m_hp = hp;
}
Agent::~Agent(){}

bool Agent::isCollisionAvoidanceWorthy() const{
    return true;
}

int Agent::getHP() const{
    return m_hp;
}
void Agent::setHP(int hp){
    m_hp += hp;
}

bool Agent::takeDamageAndPossiblyDie(int hp){
    if (getHP() <= 0){
        setDead();
        soundWhenDie();
        return true;
    }
    else{
        soundWhenHurt();
        return false;
    }
}

int Agent::soundWhenHurt(){
    return SOUND_NONE;
}
int Agent::soundWhenDie(){
    return SOUND_NONE;
}

GhostRacer::GhostRacer(StudentWorld* sw, double x, double y)
:Agent(sw, IID_GHOST_RACER, x, y, 90, 4.0, 100)
{
    m_sprays = 10;
}
GhostRacer::~GhostRacer(){}
void GhostRacer::doSomething(){
    if (isDead()){
        return;
    }
    if (this->getX() <= LEFT_EDGE && getDirection() > 90){
        setHP(-10);
        setDirection(82);
        world()->playSound(SOUND_VEHICLE_CRASH);
        drive();
        if (getHP() <= 0){
            setDead();
            world()->playSound(SOUND_PLAYER_DIE);
        }
    }
    if (this->getX() >= RIGHT_EDGE && getDirection() < 90){
        setHP(-10);
        setDirection(98);
        world()->playSound(SOUND_VEHICLE_CRASH);
        drive();
        if (getHP() <= 0){
            setDead();
            world()->playSound(SOUND_PLAYER_DIE);
        }
    }
    int ch;
    if (world()->getKey(ch)){
        switch(ch){
            case KEY_PRESS_LEFT:
            {
                if (getDirection() < 114){
                    setDirection(getDirection() + 8);
                }
                drive();
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if (getDirection() > 66){
                    setDirection(getDirection() - 8);
                }
                drive();
                break;
            }
            case KEY_PRESS_UP:
            {
                if (getVerticalSpeed() < 5)
                {setVerticalSpeed(getVerticalSpeed() + 1);}
                drive();
                break;
            }
            case KEY_PRESS_DOWN:
            {
                if (getVerticalSpeed() > 1)
                {setVerticalSpeed(getVerticalSpeed() - 1);}
                drive();
                break;
            }
            case KEY_PRESS_SPACE:
            {
                if (m_sprays > 0){
                    world()->playSound(SOUND_PLAYER_SPRAY);
                    m_sprays--;
                }
            }
            default:
                break;
        }
}
}
int GhostRacer::soundWhenDie() const{
    return SOUND_PLAYER_DIE;
}
int GhostRacer::getNumSprays() const{
    return m_sprays;
}
void GhostRacer::increaseSprays(int amt){
    m_sprays += amt;
}

void GhostRacer::spin(){
    // fix this
}

void GhostRacer::drive(){
    double max_shift_per_tick = 4.0;
    int direction = getDirection();
    double delta_x = cos(direction * PI/180) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size)
: Agent(sw, imageID, x, y, 0, size, 2)
{
    m_hspeed = 0;
    setVerticalSpeed(-4);
    setHP(2);
}
Pedestrian::~Pedestrian(){}

int Pedestrian::soundWhenHurt() const{
    return SOUND_PED_HURT;
}
int Pedestrian::soundWhenDie() const{
    return SOUND_PED_DIE;
}

int Pedestrian::getHorizSpeed() const{
    return m_hspeed;
}
void Pedestrian::setHorizSpeed(int hspeed){
    m_hspeed = hspeed;
}

void Pedestrian::moveAndPossiblyPickPlan(){
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (new_x < 0 || new_y < 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
}

HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
: Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0)
{
}
HumanPedestrian::~HumanPedestrian(){}
void HumanPedestrian::doSomething(){
    if (isDead()){
        return;
    }
    /*
     if the human pedestrian overlaps with the Ghost Racer, then the player loses a life and the level ends (you must communicate this somehow to your StudentWorld). The human pedestrian’s doSomething() method must then immediately return.
     */
    moveAndPossiblyPickPlan();
    
}
bool HumanPedestrian::beSprayedIfAppropriate(){
    return false;
}
bool HumanPedestrian::takeDamageAndPossiblyDie(int hp){
    return true;
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
: Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3.0)
{
    m_grunts = 0;
}
ZombiePedestrian::~ZombiePedestrian(){}
void ZombiePedestrian::doSomething(){
    if (isDead()){
        return;
    }
    if (abs(getX() - world()->getGhostRacer()->getX()) < 30){
        setDirection(270);
        if (getX() > world()->getGhostRacer()->getX()){
            setHorizSpeed(1);
        }
        else if (getX() < world()->getGhostRacer()->getX()){
            setHorizSpeed(-1);
        }
        else{
            setHorizSpeed(0);
        }
    }
    moveAndPossiblyPickPlan();
    decrementGrunts(); 

}
bool ZombiePedestrian::beSprayedIfAppropriate(){
    return true;
}
int ZombiePedestrian::getGrunts(){
    return m_grunts;
}
bool ZombiePedestrian::decrementGrunts(){
    if (m_grunts == 0){
        world()->playSound(SOUND_ZOMBIE_ATTACK);
        m_grunts = 20;
        m_grunts--;
        return true;
    }
    else{
        m_grunts--;
        return false;
    }
}

ZombieCab::ZombieCab(StudentWorld* sw, double x, double y)
: Agent(sw, IID_ZOMBIE_CAB, x, y, 90, 4.0, 3)
{
    
}
ZombieCab::~ZombieCab(){}
void ZombieCab::doSomething(){
    
}

bool ZombieCab::beSprayedIfAppropriate(){
    return true;
}

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
:Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1)
{
    
}
Spray::~Spray(){}
void Spray::doSomething(){
    
}

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, int dir, double size)
: Actor(sw, imageID, x, y, dir, size)
{
    
}
GhostRacerActivatedObject::~GhostRacerActivatedObject(){
    
}
bool GhostRacerActivatedObject::beSprayedIfAppropriate(){
    return false;
}

int GhostRacerActivatedObject::getSound() const{
    return SOUND_GOT_GOODIE;
}

OilSlick::OilSlick(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, 0, randInt(2, 5))
{
    
}
OilSlick::~OilSlick(){
    
}
void OilSlick::doSomething(){
    
}
void OilSlick::doActivity(GhostRacer *gr){
    
}
int OilSlick::getScoreIncrease() const{
    return 1;
}
int OilSlick::getSound() const{
    return SOUND_OIL_SLICK;
}
bool OilSlick::selfDestructs() const{
    return true;
}
bool OilSlick::isSprayable() const{
    return false;
}
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 0, 1.0)
{
    
}
HealingGoodie::~HealingGoodie(){
    
}
void HealingGoodie::doSomething(){
    
}
void HealingGoodie::doActivity(GhostRacer *gr){
    gr->setHP(10);
    setDead();
}
int HealingGoodie::getScoreIncrease() const{
    return 250;
}
bool HealingGoodie::selfDestructs() const{
    return true;
}
bool HealingGoodie::isSprayable() const{
    return true;
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 0, 2.0)
{
    
}

HolyWaterGoodie::~HolyWaterGoodie(){
    
}
void HolyWaterGoodie::doSomething(){
    
}
void HolyWaterGoodie::doActivity(GhostRacer *gr){
    gr->increaseSprays(10);
    setDead();
}
int HolyWaterGoodie::getScoreIncrease() const{
    return 50;
}
bool HolyWaterGoodie::selfDestructs() const{
    return true;
}
bool HolyWaterGoodie::isSprayable() const{
    return true;
}

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 0, 4.0)
{
    
}
SoulGoodie::~SoulGoodie(){
    
}
void SoulGoodie::doSomething(){
    
}
void SoulGoodie::doActivity(GhostRacer *gr){
    
}
int SoulGoodie::getScoreIncrease() const{
    return 50;
}
int SoulGoodie::getSound() const{
    return SOUND_GOT_SOUL;
}
bool SoulGoodie::selfDestructs() const{
    return true;
}
bool SoulGoodie::isSprayable() const{
    return false;
}
