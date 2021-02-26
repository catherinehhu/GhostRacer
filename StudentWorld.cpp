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
int lastWhiteY = 0;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_ghostracer = nullptr;
    m_actors.clear();
    m_souls = 0;
}
StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
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
    int souls2Save = getLevel() * 2 + 5;
    // add zombiecab algorithm
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
            if (getSoulsSaved() == souls2Save){
                // award bonus points to the player
                return GWSTATUS_FINISHED_LEVEL;
            }
        it++;
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
    // add new actors
    lastWhiteY += (-4 - m_ghostracer->getVerticalSpeed());
    int delta_y = new_border_y - lastWhiteY;
    if (delta_y >= SPRITE_HEIGHT){
        m_actors.push_back(new BorderLine(this, LEFT_EDGE, new_border_y, true));
        m_actors.push_back(new BorderLine(this, RIGHT_EDGE, new_border_y, true));
    }
    it = m_actors.begin();
    if (delta_y >= 4 * SPRITE_HEIGHT){
        m_actors.push_back(new BorderLine(this, ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH/3, new_border_y, false));
        m_actors.push_back(new BorderLine(this, ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH/3, new_border_y, false));
        lastWhiteY = new_border_y;
    }
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int ChanceOfLostSoul = 100;
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
    // update text
    ostringstream score;
        score  << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << (getLevel() * 2 + 5) - getSoulsSaved() << "  Lives: "<< getLives() << "  Health: " << m_ghostracer->getHP() << "  Sprays: " << m_ghostracer->getNumSprays() << "  Bonus: 5000";
        setGameStatText(score.str());
    // continue playing
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

GhostRacer* StudentWorld::getGhostRacer(){
    return m_ghostracer;
}

void StudentWorld::addActor(Actor* a){
    m_actors.push_back(a);
}

void StudentWorld::recordSoulSaved(){
    m_souls++;
}
int StudentWorld::getSoulsSaved(){
    return m_souls;
}


bool StudentWorld::sprayFirstAppropriateActor(Actor *a){
    if (a->beSprayedIfAppropriate()){
        return true;
    }
    else{
        return false;
    }
}
bool StudentWorld::overlaps(const Actor *a1, const Actor *a2) const{
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radiussum = a1->getRadius() + a2->getRadius();
    if (delta_x < radiussum*0.25 && delta_y < radiussum*0.6){
        return true;
    }
    return false;
}
bool StudentWorld::getOverlappingGhostRacer(Actor *a) const{
    double delta_x = abs(a->getX() - m_ghostracer->getX());
    double delta_y = abs(a->getY() - m_ghostracer->getY());
    double radiussum = a->getRadius() + m_ghostracer->getRadius();
    if (delta_x < radiussum*0.25 && delta_y < radiussum*0.6){
        return true;
    }
    return false;
}

bool StudentWorld::closest(int lane){
    list<Actor*>::iterator bottom = m_actors.begin();
    list<Actor*>::iterator top = m_actors.begin();
    list<Actor*>::iterator it = m_actors.begin();
    double topDistance = 0;
    double bottomDistance = VIEW_HEIGHT;
    while (it != m_actors.end()){
        if ((*it)->isCollisionAvoidanceWorthy() && (*it)->getY() < bottomDistance && (*it)->getX() > LEFT_EDGE + lane * ROAD_WIDTH/3 && (*it)->getX() < LEFT_EDGE + (lane +1) * ROAD_WIDTH/3){
            bottom = it;
            bottomDistance = (*it)->getY();
        }
        if ((*it)->isCollisionAvoidanceWorthy() && (*it)->getY() > topDistance && (*it)->getX() > LEFT_EDGE + lane * ROAD_WIDTH/3 && (*it)->getX() < LEFT_EDGE + (lane +1) * ROAD_WIDTH/3){
            top = it;
            topDistance = (*it)->getY();
        }
    }
    double x = 0;
    double y = 0;
    if (bottom == m_actors.end() || bottomDistance > VIEW_HEIGHT/3){
        switch (lane){
            case 0:
                x = ROAD_CENTER - ROAD_WIDTH/3;
                y = SPRITE_HEIGHT / 2;
                break;
            case 1:
                x = ROAD_CENTER;
                y = SPRITE_HEIGHT / 2;
                break;
            case 2:
                x = ROAD_CENTER + ROAD_WIDTH/3;
                y = SPRITE_HEIGHT / 2;
                break;
            default:
                break;
        }
        ZombieCab* zombiecab = new ZombieCab(this, x, y);
        addActor(zombiecab);
        return true;
    }
    if (top == m_actors.end() || topDistance < 2 * VIEW_HEIGHT/3){
        switch (lane){
            case 0:
                x = ROAD_CENTER - ROAD_WIDTH/3;
                y = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                break;
            case 1:
                x = ROAD_CENTER;
                y = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                break;
            case 2:
                x = ROAD_CENTER + ROAD_WIDTH/3;
                y = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                break;
            default:
                break;
        }
        ZombieCab* zombiecab = new ZombieCab(this, x, y);
        addActor(zombiecab);
        return true;
    }
    return false;
}

