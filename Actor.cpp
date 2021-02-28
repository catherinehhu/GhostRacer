#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
const double PI = 3.14159265;
const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

Actor::Actor(StudentWorld* sw, int imageID, double x, double y, int dir = 0, double size = 0, unsigned int depth = 0)
:GraphObject(imageID, x, y, dir, size, depth)
{
    m_dead = false;
    m_vspeed = 0;
    m_world = sw;
}
Actor::~Actor(){}
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
bool Actor::isCollisionAvoidanceWorthy() const{
    return true;
}
void Actor::getSprayed(){
    return;
}
bool Actor::beSprayedIfAppropriate() {
    return false;
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
    if(getHP() <= 0){
        setDead();
    }
}
void Agent::getSprayed(){
    return;
}
bool Agent::takeDamageAndPossiblyDie(int hp){
    setHP(hp);
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
    drive();
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
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if (getDirection() > 66){
                    setDirection(getDirection() - 8);
                }
                break;
            }
            case KEY_PRESS_UP:
            {
                if (getVerticalSpeed() < 5)
                {setVerticalSpeed(getVerticalSpeed() + 1);}
                break;
            }
            case KEY_PRESS_DOWN:
            {
                if (getVerticalSpeed() > 1)
                {setVerticalSpeed(getVerticalSpeed() - 1);}
                break;
            }
            case KEY_PRESS_SPACE:
            {
                if (m_sprays > 0){
                    world()->addActor(new Spray(world(), SPRITE_HEIGHT * cos(getDirection() * PI/180) + getX(), SPRITE_HEIGHT * sin(getDirection() * PI/180) + getY(), getDirection()));
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
    setPlan(0);
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
    decrementPlan();
    if (getPlan() > 0){
        return;
    }
    else{
        int chooseSpeed = 0;
        while (chooseSpeed == 0){
            chooseSpeed = randInt(-3,3);
        }
        setHorizSpeed(chooseSpeed);
        setPlan(randInt(4, 32));
        if (getHorizSpeed() < 0){
            setDirection(180);
        }
        else{
            setDirection(0);
        }
    }
}
void Pedestrian::decrementPlan(){
    m_plan--;
}
void Pedestrian::setPlan(int plan){
    m_plan = plan;
}
int Pedestrian::getPlan(){
    return m_plan;
}
void Pedestrian::move(){
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
HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
: Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0)
{}
HumanPedestrian::~HumanPedestrian(){}
void HumanPedestrian::doSomething(){
    if (isDead()){
        return;
    }
    if (world()->getOverlappingGhostRacer(this)){
        setDead();
        world()->getGhostRacer()->setDead();
        return;
    }
    else{
        move();
    }
    moveAndPossiblyPickPlan();
}
bool HumanPedestrian::beSprayedIfAppropriate(){
    return true;
}
void HumanPedestrian::getSprayed(){
    takeDamageAndPossiblyDie(0);
}
bool HumanPedestrian::takeDamageAndPossiblyDie(int hp){
    int currDirection = getDirection();
    int currHSpeed = getHorizSpeed();
    setDirection(currDirection * -1);
    setHorizSpeed(currHSpeed * -1);
    world()->playSound(SOUND_PED_DIE);
    setHP(hp);
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
    if (world()->getOverlappingGhostRacer(this)){
        world()->getGhostRacer()->setHP(-5);
        takeDamageAndPossiblyDie(-2);
        setDead(); 
        world()->increaseScore(150);
        return; 
    }
    if (abs(getX() - world()->getGhostRacer()->getX()) < 30 && getY() > world()->getGhostRacer()->getY()){
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
        decrementGrunts();
    }
    move();
    moveAndPossiblyPickPlan();
}
bool ZombiePedestrian::beSprayedIfAppropriate(){
    return true;
}
void ZombiePedestrian::getSprayed(){
    takeDamageAndPossiblyDie(-1);
}
bool ZombiePedestrian::takeDamageAndPossiblyDie(int hp){
    setHP(hp);
    if (isDead()){
        world()->playSound(SOUND_PED_DIE);
                if (randInt(1,5) == 1 && hp == -1){
                    HealingGoodie* healinggoodie = new HealingGoodie(world(), getX(), getY());
                    world()->addActor(healinggoodie);
                }
                world()->increaseScore(200);
        return true;
            }
    else{
        world()->playSound(SOUND_PED_HURT);
        return false;
    }
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
: Pedestrian(sw, IID_ZOMBIE_CAB, x, y, 4.0)
{
    setHP(3);
    setHorizSpeed(0);
    setDirection(90);
    m_damage = false;
}
ZombieCab::~ZombieCab(){}
void ZombieCab::doSomething(){
    if (isDead()){
        return;
    }
    if (world()->getOverlappingGhostRacer(this))
    {
        if (checkDamage()){
            ;
        }
        else{
            world()->playSound(SOUND_VEHICLE_CRASH);
            world()->getGhostRacer()->setHP(-20);
            if (getX() <= world()->getGhostRacer()->getX()){
                setHorizSpeed(-5);
                setDirection(120 + randInt(0, 19));
                setDirection(60-randInt(0, 19));
            }
            else if (getX() > world()->getGhostRacer()->getX()){
                setHorizSpeed(5);
                setDirection(60-randInt(0, 19));
            }
            doneDamage();
        }
    }
    move();
    if (getVerticalSpeed() > world()->getGhostRacer()->getVerticalSpeed() && world()->checkCollision(this) == 0.5){
        setVerticalSpeed(getVerticalSpeed() - 0.5);
    }
    else if (getVerticalSpeed() <=  world()->getGhostRacer()->getVerticalSpeed() && world()->checkCollision(this) == 1.5){
        setVerticalSpeed(getVerticalSpeed() - 0.5);
    }
    moveAndPossiblyPickPlan();
}

bool ZombieCab::checkDamage(){
    return m_damage;
}
void ZombieCab::doneDamage(){
    m_damage = true;
}
void ZombieCab::moveAndPossiblyPickPlan(){
    decrementPlan();
    if (getPlan() > 0){
        return;
    }
    else{
        setPlan(randInt(4,32));
        setVerticalSpeed(getVerticalSpeed() + randInt(-2,2));
    }
}
bool ZombieCab::beSprayedIfAppropriate(){
    return true;
    }
void ZombieCab::getSprayed(){
    takeDamageAndPossiblyDie(-1);
}
bool ZombieCab::takeDamageAndPossiblyDie(int hp){
    setHP(hp);
    if (isDead()){
        world()->playSound(SOUND_VEHICLE_DIE);
                if (randInt(1,5) == 1){
                    OilSlick* oilslick = new OilSlick(world(), getX(), getY());
                    world()->addActor(oilslick);
                }
                world()->increaseScore(200);
                return true;
            }
    else{
        world()->playSound(SOUND_VEHICLE_HURT);
        return false;
    }
}

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
:Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1)
{
    m_pixels = 160;
}
Spray::~Spray(){}
void Spray::doSomething(){
    if (isDead()){
        return; 
    }
    if (world()->sprayOverlap(this)){
        setDead();
        return;
    }
    moveForward(SPRITE_HEIGHT);
    decrementPixels(SPRITE_HEIGHT);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
    if (getPixels() <= 0){
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
void GhostRacerActivatedObject::getSprayed(){
    if (beSprayedIfAppropriate() && selfDestructs()){
        setDead();
    }
}
OilSlick::OilSlick(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, 0, randInt(2, 5), 1) {}
OilSlick::~OilSlick(){}
void OilSlick::doSomething(){
    move();
    if (world()->getOverlappingGhostRacer(this)){
        doActivity(world()->getGhostRacer());
    }
}
void OilSlick::doActivity(GhostRacer *gr){
    setDead();
    world()->playSound(getSound());
    gr->spin();
}
int OilSlick::getScoreIncrease() const{
    return 1;
}
int OilSlick::getSound() const{
    return SOUND_OIL_SLICK;
}
bool OilSlick::selfDestructs() const{
    return false;
}
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 0, 1.0, 2)
{}
HealingGoodie::~HealingGoodie(){}
void HealingGoodie::doSomething(){
    move();
    if (world()->getOverlappingGhostRacer(this)){
        this->doActivity(world()->getGhostRacer());
    }
}
void HealingGoodie::doActivity(GhostRacer *gr){
    world()->getGhostRacer()->setHP(10);
    setDead();
    world()->playSound(getSound());
    world()->increaseScore(getScoreIncrease());
}
int HealingGoodie::getScoreIncrease() const{
    return 250;
}
bool HealingGoodie::selfDestructs() const{
    return true;
}
bool HealingGoodie::beSprayedIfAppropriate() {
    return true;
}
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 0, 2.0, 2)
{
    setDirection(90); 
}
HolyWaterGoodie::~HolyWaterGoodie(){}
void HolyWaterGoodie::doSomething(){
    move();
    if (world()->getOverlappingGhostRacer(this)){
        this->doActivity(world()->getGhostRacer());
    }
}
void HolyWaterGoodie::doActivity(GhostRacer *gr){
    setDead();
    world()->getGhostRacer()->increaseSprays(10);
    world()->playSound(getSound());
    world()->increaseScore(getScoreIncrease());
}
int HolyWaterGoodie::getScoreIncrease() const{
    return 50;
}
bool HolyWaterGoodie::selfDestructs() const{
    return true;
}
bool HolyWaterGoodie::beSprayedIfAppropriate() {
    return true;
}
SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 0, 4.0, 2)
{}
SoulGoodie::~SoulGoodie(){}
void SoulGoodie::doSomething(){
    move();
    if (world()->getOverlappingGhostRacer(this)){
        this->doActivity(world()->getGhostRacer());
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
    world()->recordSoulSaved();
    setDead();
    world()->playSound(getSound());
    world()->increaseScore(getScoreIncrease());
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
