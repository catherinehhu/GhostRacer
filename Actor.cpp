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
bool Actor::isDead() const{ // returns alive/dead state of each actor
return m_dead;
}
void Actor::setDead(){ // kills the actor
    m_dead = true;
}
StudentWorld* Actor::world() const{ // allows access to student world
    return m_world;
}
double Actor::getVerticalSpeed() const{ // return vertical speed of each object
    return m_vspeed;
}
void Actor::setVerticalSpeed(double speed){ // update vertical speed
    m_vspeed = speed;
}
bool Actor::isCollisionAvoidanceWorthy() const{ // will avoid collisions
    return true;
}
void Actor::getSprayed(){ // do something if actor gets sprayed
    return;
}
bool Actor::beSprayedIfAppropriate() { // cannot be sprayed
    return false;
}
BorderLine::BorderLine(StudentWorld* sw, double x, double y, bool isYellow)
:Actor(sw, ((isYellow) ? IID_YELLOW_BORDER_LINE : IID_WHITE_BORDER_LINE), x, y, 0, 2.0, 2)
{
    setVerticalSpeed(-4.0);
}
BorderLine::~BorderLine(){}
void BorderLine::doSomething(){ // follow spec algorithm to move based on ghostracer speed
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
Agent::Agent(StudentWorld* sw, int imageID, double x, double y, int dir, double size, int hp)
: Actor(sw, imageID, x, y, dir, size, 0)
{
    m_hp = hp;
}
Agent::~Agent(){}
bool Agent::isCollisionAvoidanceWorthy() const{ // all agents will avoid collisions
    return true;
}
int Agent::getHP() const{ // returns hp of an agent
    return m_hp;
}
void Agent::setHP(int hp){ // updates hp of an agent
    if (m_hp + hp <= 100)
    m_hp += hp;
    if(getHP() <= 0){
        setDead();
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
    if (isDead()){ // return if dead
        return;
    }
    drive();
    if (this->getX() <= LEFT_EDGE && getDirection() > 90){ // update direction/change hp if it crashes into border
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
    if (world()->getKey(ch)){ // determine direction/speed based on key input
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
int GhostRacer::getNumSprays() const{ // return num of sprays left
    return m_sprays;
}
void GhostRacer::increaseSprays(int amt){ // update sprays according to amount
    m_sprays += amt;
}
void GhostRacer::spin(){ // response to oil slicks
    int currDirection = getDirection();
    int change = randInt(5, 20);
    (currDirection + change > 120) ? setDirection(currDirection - change) : setDirection(currDirection + change);
}
void GhostRacer::drive(){ // movement function following spec, determine new position
    double max_shift_per_tick = 4.0;
    int direction = getDirection();
    double delta_x = cos(direction * PI/180) * max_shift_per_tick; // convert to degrees—cosine in c++ is in radians
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
void Pedestrian::moveAndPossiblyPickPlan(){ // decrement plan, update speed of peds
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
bool Pedestrian::beSprayedIfAppropriate(){ // all peds can be sprayed
    return true;
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
void Pedestrian::move(){ // movement function shared by zombie cab/zombie ped/human ped
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
: Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0) {}
HumanPedestrian::~HumanPedestrian(){}
void HumanPedestrian::doSomething(){
    if (isDead()){ // check if dead
        return;
    }
    if (world()->getOverlappingGhostRacer(this)){ // check if ghostracer crashed into it
        setDead();
        world()->getGhostRacer()->setDead();
        return;
    }
    else{
        move(); // otherwise move
    }
    moveAndPossiblyPickPlan();
}
void HumanPedestrian::getSprayed(){ // flip directions when sprayed
    int currDirection = getDirection();
    int currHSpeed = getHorizSpeed();
    setDirection(currDirection * -1);
    setHorizSpeed(currHSpeed * -1);
}
ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
: Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3.0)
{
    m_grunts = 0;
}
ZombiePedestrian::~ZombiePedestrian(){}
void ZombiePedestrian::doSomething(){
    if (isDead()){ // check if dead
        return;
    }
    if (world()->getOverlappingGhostRacer(this)){ // if there's overlap, cause damage
        world()->getGhostRacer()->setHP(-5);
        setHP(-2);
        setDead(); 
        world()->increaseScore(150);
        return; 
    }
    if (abs(getX() - world()->getGhostRacer()->getX()) < 30 && getY() > world()->getGhostRacer()->getY()){ // check proximity to ghostracer, update speed and direction accordingly
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
void ZombiePedestrian::getSprayed(){
    setHP(-1); // reduce by 1 hp
    if (isDead()){ // play appropriate sound and potentially create new healinggoodie
        world()->playSound(SOUND_PED_DIE);
                if (randInt(1,5) > 0){
                    HealingGoodie* healinggoodie = new HealingGoodie(world(), getX(), getY());
                    world()->addActor(healinggoodie);
                }
                world()->increaseScore(200);
            }
    else{
        world()->playSound(SOUND_PED_HURT);
}
}
int ZombiePedestrian::getGrunts(){
    return m_grunts;
}
void ZombiePedestrian::decrementGrunts(){ // checks if enough ticks have passed to play zombie attack sound
    if (m_grunts == 0){
        world()->playSound(SOUND_ZOMBIE_ATTACK);
        m_grunts = 20;
        m_grunts--;
    }
    else{
        m_grunts--;
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
    if (isDead()){ // check if dead
        return;
    }
    if (world()->getOverlappingGhostRacer(this)){ // check for overlap
        if (checkDamage()){
            ;
        }
        else{ // determine direction accordingly
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
    move(); // follow movement plan outlined in spec
    if (getVerticalSpeed() > world()->getGhostRacer()->getVerticalSpeed() && world()->checkCollision(this) == 0.5){
        setVerticalSpeed(getVerticalSpeed() - 0.5);
        return;
    }
    else if (getVerticalSpeed() <=  world()->getGhostRacer()->getVerticalSpeed() && world()->checkCollision(this) == -0.5){
        setVerticalSpeed(getVerticalSpeed() - 0.5);
        return;
    }
    moveAndPossiblyPickPlan();
}
bool ZombieCab::checkDamage(){
    return m_damage;
}
void ZombieCab::doneDamage(){
    m_damage = true;
}
void ZombieCab::moveAndPossiblyPickPlan(){ // movement plan for zombie cab is different from zombie ped and human ped
    decrementPlan();
    if (getPlan() > 0){
        return;
    }
    else{
        setPlan(randInt(4,32));
        setVerticalSpeed(getVerticalSpeed() + randInt(-2,2));
    }
}
void ZombieCab::getSprayed(){ // if zombie cab gets sprayed, reduce hp by 1
    setHP(-1);
    if (isDead()){ // play appropriate sound, potentially make new oilslick
        world()->playSound(SOUND_VEHICLE_DIE);
                if (randInt(1,5) == 1){
                    OilSlick* oilslick = new OilSlick(world(), getX(), getY());
                    world()->addActor(oilslick);
                }
                world()->increaseScore(200);
            }
    else{
        world()->playSound(SOUND_VEHICLE_HURT);
    }
}
Spray::Spray(StudentWorld* sw, double x, double y, int dir)
:Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1)
{
    m_pixels = 160;
}
Spray::~Spray(){}
void Spray::doSomething(){
    if (isDead()){ // check if dead
        return; 
    }
    if (world()->sprayOverlap(this)){ // find overlap
        setDead();
        return;
    }
    moveForward(SPRITE_HEIGHT); // move distance
    decrementPixels(SPRITE_HEIGHT); // if it exceeds 160 pixels, will be killed and stopped
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
void GhostRacerActivatedObject::move(){ // follows movement algorithm for all goodies
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
void GhostRacerActivatedObject::getSprayed(){ // if goodie can be sprayed and will be destroyed, kill it
    if (beSprayedIfAppropriate() && selfDestructs()){
        setDead();
    }
}
void GhostRacerActivatedObject::doSomething(){ // each goodie will follow same dosomething pattern except the soulgoodie
    move();
    if (world()->getOverlappingGhostRacer(this)){
        this->doActivity(world()->getGhostRacer());
    }
}
OilSlick::OilSlick(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, 0, randInt(2, 5), 1) {}
OilSlick::~OilSlick(){}
void OilSlick::doActivity(GhostRacer *gr){ // called during dosomething, makes the ghostracer spin
    setDead();
    world()->playSound(getSound());
    gr->spin();
}
int OilSlick::getScoreIncrease() const{
    return 0;
}
int OilSlick::getSound() const{
    return SOUND_OIL_SLICK;
}
bool OilSlick::selfDestructs() const{
    return false;
}
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
:GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 0, 1.0, 2) {}
HealingGoodie::~HealingGoodie(){}
void HealingGoodie::doActivity(GhostRacer *gr){ // called during dosomething, increases ghostracer hp, plays sound, increases score
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
void HolyWaterGoodie::doActivity(GhostRacer *gr){ // called during dosomething, increases ghostracer water, plays sound, increases score
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
:GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 0, 4.0, 2) {}
SoulGoodie::~SoulGoodie(){}
void SoulGoodie::doSomething(){ // moves, updates direction and allows for rotation
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
void SoulGoodie::doActivity(GhostRacer *gr){ // called during dosomething, increases ghostracer souls counter, plays sound, increases score 
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
