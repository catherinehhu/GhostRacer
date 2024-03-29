#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

const int M = VIEW_HEIGHT/(4 * SPRITE_HEIGHT);
const int N = VIEW_HEIGHT/SPRITE_HEIGHT;
const double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
const int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_ghostracer = nullptr;
    m_actors.clear();
}
StudentWorld::~StudentWorld(){
    cleanUp();
}
int StudentWorld::init()
{
    m_souls2save = getLevel() * 2 + 5;
    m_lastWhiteY = 0;
    m_bonus = 5000;
    m_ghostracer = new GhostRacer(this, 128, 32);
    for (int j = 0; j < N; j++){
        m_actors.push_back(new BorderLine(this, LEFT_EDGE, j * SPRITE_HEIGHT, true));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE, j * SPRITE_HEIGHT, true));
    }
    for (int j = 0; j < M; j++){
        m_actors.push_back(new BorderLine(this, LEFT_EDGE + ROAD_WIDTH/3, j * 4 * SPRITE_HEIGHT, false));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE - ROAD_WIDTH/3, j * 4 * SPRITE_HEIGHT, false));
    }
    return GWSTATUS_CONTINUE_GAME;
}
int StudentWorld::move()
{
    m_ghostracer->doSomething();
    list<Actor*>::iterator it = m_actors.begin();
    for (it = m_actors.begin(); it != m_actors.end(); )
    {
        if(!(*it)->isDead())
        {
        (*it)->doSomething();
        }
            if (m_ghostracer->isDead()){
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (getSoulsLeft() == 0){
                increaseScore(getBonus());
                return GWSTATUS_FINISHED_LEVEL;
            }
        it++;
    }
    // add new actors
    m_lastWhiteY += (-4 - m_ghostracer->getVerticalSpeed());
    int delta_y = new_border_y - m_lastWhiteY;
    if (delta_y >= SPRITE_HEIGHT){
        m_actors.push_back(new BorderLine(this, LEFT_EDGE, new_border_y, true));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE, new_border_y, true));
    }
    it = m_actors.begin();
    if (delta_y >= 4 * SPRITE_HEIGHT){
        m_actors.push_back(new BorderLine(this, ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH/3, new_border_y, false));
        m_actors.push_back(new BorderLine(this, ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH/3, new_border_y, false));
        m_lastWhiteY = new_border_y;
    }
    // remove newly dead actors
    it = m_actors.begin();
    while (it != m_actors.end()){
        if ((*it)->isDead()){
            delete *it;
            list<Actor*>::iterator toErase = it;
            it++;
            m_actors.erase(toErase);
        }
        else{
            it++;
        }
    }
    // create new actors, determine probabilities 
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceVehicle - 1) == 0){
        createZombieCab();
    }
    if (randInt(0, ChanceOilSlick - 1) == 0){
        OilSlick* oilslick = new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        addActor(oilslick);
    }
    if (randInt(0, ChanceZombiePed - 1) == 0){
        ZombiePedestrian* zombieped = new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT);
        addActor(zombieped);
    }
    if (randInt(0, ChanceHumanPed - 1) == 0){
        HumanPedestrian* humanped = new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT);
        addActor(humanped);
    }
    if (randInt(0, ChanceOfHolyWater - 1) == 0){
       HolyWaterGoodie* holywatergoodie = new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        addActor(holywatergoodie);
    }
    if (randInt(0, ChanceOfLostSoul - 1) == 0){
        SoulGoodie* soulgoodie = new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT);
        addActor(soulgoodie);
    }
    decrementBonus();
    // update text
    ostringstream score;
        score  << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << getSoulsLeft() << "  Lives: "<< getLives() << "  Health: " << m_ghostracer->getHP() << "  Sprays: " << m_ghostracer->getNumSprays() << "  Bonus:  " << getBonus();
        setGameStatText(score.str());
    if (m_ghostracer->isDead()){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    else
    return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::cleanUp()
{
    while (!m_actors.empty())
        {
            delete m_actors.back();
            m_actors.pop_back();
        }
    delete m_ghostracer;
    m_ghostracer = nullptr; 
}
GhostRacer* StudentWorld::getGhostRacer() const{
    return m_ghostracer;
}
void StudentWorld::addActor(Actor* a){
    m_actors.push_back(a);
}
void StudentWorld::recordSoulSaved(){ // one less soul to save
    m_souls2save--;
}
int StudentWorld::getSoulsLeft(){ // return for string
    return m_souls2save;
}
bool StudentWorld::overlaps(const Actor *a1, const Actor *a2) const{ // checks for general overlap using formula from spec
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radiussum = a1->getRadius() + a2->getRadius();
    if (delta_x < radiussum*0.25 && delta_y < radiussum*0.6){
        return true;
    }
    return false;
}
bool StudentWorld::getOverlappingGhostRacer(const Actor *a) const{ // checks if actor overlaps with ghostracer
    if (overlaps(a, getGhostRacer())){
        return true;
    }
    return false;
}
bool StudentWorld::sprayOverlap(const Actor* spray) { // checks if spray overlaps
    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if (!(*it)->isDead() && (*it)->beSprayedIfAppropriate() && overlaps(spray, *it))
        {
            (*it)->getSprayed(); // spray once then break out of loop
            return true;
        }
    }
    return false;
}
double StudentWorld::checkCollision(const Actor *actor){ // checks zombiecab against other collision avoidance worthy actors
    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->isCollisionAvoidanceWorthy()){
            if ((*it)->getY() > actor->getY() && (*it)->getY() - actor->getY() < 96)
            {
                return 0.5; // will increase zombiecab speed
            }
            if (actor->getY() > (*it)->getY() && actor->getY() - (*it)->getY() < 96)
            {
                return -0.5; // will decrease zombiecab speed
            }
        }
    }
    return 1;
}
void StudentWorld::createZombieCab(){
    int cur_lane = randInt(0,2);
    list<Actor*>::iterator bottom = m_actors.begin();
    list<Actor*>::iterator top = m_actors.begin();
    list<Actor*>::iterator it = m_actors.begin();
    double topDistance = 0;
    double bottomDistance = VIEW_HEIGHT;
    for (int i = 0; i < 3; i++){ // will search up to 3 lanes
        for (it = m_actors.begin(); it != m_actors.end(); it++){
            if (it == m_actors.begin() && m_ghostracer->getX() > LEFT_EDGE + cur_lane*ROAD_WIDTH/3 && m_ghostracer->getX() < LEFT_EDGE + (cur_lane + 1)*ROAD_WIDTH/3){ // gets checked at the beginning, looks for potential ghostracer in the current lane
                if ((m_ghostracer)->getY() < bottomDistance){
                    bottomDistance = (m_ghostracer)->getY();
                    bottom = m_actors.end();
                }
                if ((m_ghostracer)->getY() > topDistance){
                    topDistance = (*it)->getY();
                    top = m_actors.end();
                }
            }
            if ((*it)->getX() > LEFT_EDGE + cur_lane*ROAD_WIDTH/3 && (*it)->getX() < LEFT_EDGE + (cur_lane + 1)*ROAD_WIDTH/3 && (*it)->isCollisionAvoidanceWorthy()){ // in the lane
                if ((*it)->getY() < bottomDistance){ // closest collision worthy object from the bottom
                    bottomDistance = (*it)->getY();
                    bottom = it;
                }
                if ((*it)->getY() > topDistance){ // closest collision worthy object from the top
                    topDistance = (*it)->getY();
                    top = it;
                }
            }
            if (bottomDistance <= VIEW_HEIGHT/3 || topDistance >= VIEW_HEIGHT * 2 /3){ // bad lane
                cur_lane++;
                cur_lane = cur_lane%3; // updates value of cur_lane
                break;
            }
        }
        if (bottom == m_actors.begin() || bottomDistance > (VIEW_HEIGHT/3) || top == m_actors.begin() || topDistance < (VIEW_HEIGHT * 2)/3){ // after one loop thru the container and a good lane is found, break from outer loop
            break;
        }
    }
    double x = 0;
    double y = 0;
    double speed = 0;
    switch(cur_lane){ // update x depending on the appropriate lane
        case 0:
            x = ROAD_CENTER - ROAD_WIDTH/3;
            break;
        case 1:
            x = ROAD_CENTER;
            break;
        case 2:
            x = ROAD_CENTER + ROAD_WIDTH/3;
            break;
        default:
            break;
    }
    if (bottom == m_actors.begin() || bottomDistance > (VIEW_HEIGHT/3)){
        y = SPRITE_HEIGHT/2;
        speed = getGhostRacer()->getVerticalSpeed() + randInt(2,4);
    }
    else if (top == m_actors.begin() || topDistance < (VIEW_HEIGHT * 2)/3){
        y = VIEW_HEIGHT - SPRITE_HEIGHT/2;
        speed = getGhostRacer()->getVerticalSpeed() - randInt(2,4);
    }
    else{
        return;
    }
    ZombieCab* zombiecab = new ZombieCab(this, x, y);
    addActor(zombiecab);
    zombiecab->setVerticalSpeed(speed);
    }

void StudentWorld::decrementBonus(){ // gets called during every tick
    if (m_bonus > 0)
    m_bonus--;
}
int StudentWorld::getBonus(){ // will be used to determine final score
    return m_bonus;
}
