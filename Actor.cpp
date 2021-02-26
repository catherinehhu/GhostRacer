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
:Actor(sw, ((isYellow) ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE), x, y, 0, 2.0, 2)
{
    setVerticalSpeed(-4.0);
}
BorderLine::~BorderLine(){}
void BorderLine::doSomething(){
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX();
    moveTo(new_x, new_y);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
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
    if (m_hp + hp <= 100)
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
                
                /*
                 Add a new holy water spray object SPRITE_HEIGHT pixels
                 directly in front of the Ghost Racer (in the same direction Ghost Racer is facing) into their StudentWorld object. Hint: The cos() and sin() functions can be used to determine the proper delta_x and delta_y in front of Ghost Racer where to place the new holy water projectile.
                 */
            }
            default:
                drive();
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
    int currDirection = getDirection();
    int change = randInt(5, 20);
    (currDirection + change > 120) ? setDirection(currDirection - change) : setDirection(currDirection + change);
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
    else{
//    moveAndPossiblyPickPlan();
        double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
        double horiz_speed = getHorizSpeed();
        double new_y = getY() + vert_speed;
        double new_x = getX() + horiz_speed;
        moveTo(new_x, new_y);
        if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
        {
            setDead();
            return;
        }
    }
    int negative = randInt(-3,-1);
    int positive = randInt(1,3);
    setHorizSpeed(randInt(negative, positive));
    if (getHorizSpeed() < 0){
        setDirection(180);
    }
    else{
        setDirection(0);
    }
}
bool HumanPedestrian::beSprayedIfAppropriate(){
    int currDirection = getDirection();
    int currHSpeed = getHorizSpeed();
    setDirection(currDirection * -1);
    setHorizSpeed(currHSpeed * -1);
    world()->playSound(SOUND_PED_HURT);
    return true;
}
bool HumanPedestrian::takeDamageAndPossiblyDie(int hp){
    setDead();
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
    /*
     If the zombie pedestrian overlaps with the Ghost Racer:
     a. The Ghost Racer must receive 5 points of damage.
     b. The zombie pedestrian will be damaged6 with 2 hit points of damage. See
     the Other Circumstances section below for what it means to damage a
     zombie pedestrian.
     c. The zombie pedestrian must immediately return and do nothing else.
     */
    
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
//    moveAndPossiblyPickPlan();
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
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
    m_hspeed = 0;
}
ZombieCab::~ZombieCab(){}
int ZombieCab::getHSpeed(){
    return m_hspeed;
}
void ZombieCab::setHSpeed(int hspeed){
    m_hspeed = hspeed;
}
void ZombieCab::doSomething(){
    if (isDead()){
        return;
    }
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double horiz_speed = getHSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return;
    }

}

bool ZombieCab::beSprayedIfAppropriate(){
    return true;
}

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
:Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1)
{
    m_pixels = 0;
}
Spray::~Spray(){}
void Spray::doSomething(){
    if (isDead()){
        return; 
    }
    /*
     Check to see if it has activated. A holy water projectile activates if an object that is affected by projectiles overlaps with it (e.g., zombie peds, zombie cabs, and some types of goodies). If multiple objects overlap with a holy water projectile, then you may pick any one and ignore the others. If the holy water projectile overlaps with an affected object, the holy water spray must:
     a. Attempt to damage the other object with 1 hit point of damage.
     b. Set its own status to not-alive, so it will be removed by StudentWorld later
     in this tick.
     c. Return immediately.
     */
    moveForward(SPRITE_HEIGHT);
    decrementPixels(SPRITE_HEIGHT);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
    if (getPixels() > 160){
        setDead();
        return;
    }
}
bool Spray::isCollisionAvoidanceWorthy() const{
    return false;
}
int Spray::getPixels(){
    return m_pixels;
}
void Spray::decrementPixels(int pixels){
    m_pixels -= pixels;
}

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int depth)
: Actor(sw, imageID, x, y, dir, size, depth)
{
    setVerticalSpeed(-4);
}
GhostRacerActivatedObject::~GhostRacerActivatedObject(){}
bool GhostRacerActivatedObject::beSprayedIfAppropriate(){
    return false;
}
int GhostRacerActivatedObject::getSound() const{
    return SOUND_GOT_GOODIE;
}
bool GhostRacerActivatedObject::isCollisionAvoidanceWorthy() const{
    return false;
}
void GhostRacerActivatedObject::move(){
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double horiz_speed = 0;
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT){
        setDead();
        return;
    }
}

OilSlick::OilSlick(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, 0, randInt(2, 5), 1) {}
OilSlick::~OilSlick(){}
void OilSlick::doSomething(){
    move();
    if (world()->getOverlappingGhostRacer(this)){
        world()->getGhostRacer()->setHP(-10);
        setDead();
        world()->playSound(SOUND_OIL_SLICK);
        doActivity(world()->getGhostRacer());
    }
}
void OilSlick::doActivity(GhostRacer *gr){
    gr->spin();
}
//int OilSlick::getScoreIncrease() const{
//    return 1;
//}
int OilSlick::getSound() const{
    return SOUND_OIL_SLICK;
}
bool OilSlick::selfDestructs() const{
    return false;
}
bool OilSlick::isSprayable() const{
    return false;
}

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 0, 1.0, 2)
{}
HealingGoodie::~HealingGoodie(){}
void HealingGoodie::doSomething(){
    // can be destroyed if overlaps with holy water projectile, will destroy it
    move();
    if (world()->getOverlappingGhostRacer(this)){
        world()->getGhostRacer()->setHP(10);
        setDead();
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(getScoreIncrease());
    }
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
:GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 0, 2.0, 2)
{}

HolyWaterGoodie::~HolyWaterGoodie(){}
void HolyWaterGoodie::doSomething(){
    // can be destroyed if overlaps with holy water projectile, will destroy it
    move();
    if (world()->getOverlappingGhostRacer(this)){
        setDead();
        world()->getGhostRacer()->increaseSprays(10);
        world()->playSound(SOUND_GOT_GOODIE);
        world()->increaseScore(getScoreIncrease());
    }
}
void HolyWaterGoodie::doActivity(GhostRacer *gr){}
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
:GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 0, 4.0, 2)
{}
SoulGoodie::~SoulGoodie(){}
void SoulGoodie::doSomething(){
    move();
    if (world()->getOverlappingGhostRacer(this)){
        world()->recordSoulSaved();
        setDead();
        world()->playSound(SOUND_GOT_SOUL);
        world()->increaseScore(getScoreIncrease());
    }
    int currDirection = getDirection();
    if (currDirection >= 349){
        setDirection(359-currDirection + 10);
    }
    else{
        setDirection(currDirection + 10);
    }
}
void SoulGoodie::doActivity(GhostRacer *gr){
    setDead();
    world()->playSound(SOUND_GOT_SOUL);
}
int SoulGoodie::getScoreIncrease() const{
    return 100;
}
int SoulGoodie::getSound() const{
    return SOUND_GOT_SOUL;
}
bool SoulGoodie::selfDestructs() const{
    return false;
}
bool SoulGoodie::isSprayable() const{
    return false;
}