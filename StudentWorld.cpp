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
    
    // add zombiecab algorithm
    // checking for souls saved to return level-finished
    
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int ChanceOfLostSoul = 100;
    
    m_ghostracer->doSomething();
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()){
        if (!(*it)->isDead()){
        (*it)->doSomething();
        it++;
        }
        else{
            it++;
        }
    }
    if (m_ghostracer->isDead()){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    // remove newly dead actors
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
    
    if (randInt(0, ChanceVehicle - 1) == 0){
        int lane = randInt(0, 2); // 0 is left, 1 is middle, 2 is right lane
        
        m_actors.push_back(new ZombieCab(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    
    if (randInt(0, ChanceOilSlick - 1) == 0){
        m_actors.push_back(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    if (randInt(0, ChanceZombiePed - 1) == 0){
        m_actors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    if (randInt(0, ChanceHumanPed - 1) == 0){
        m_actors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }
    if (randInt(0, ChanceOfHolyWater - 1) == 0){
        m_actors.push_back(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    if (randInt(0, ChanceOfLostSoul - 1) == 0){
        m_actors.push_back(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    
    // update text
    ostringstream score;
        score  << "Score: " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << getLevel() * 2 + 5 << "  Lives: "<< getLives() << "  Health: " << m_ghostracer->getHP() << "  Sprays: " << m_ghostracer->getNumSprays() << "  Bonus: 5000";
        setGameStatText(score.str());
    // continue playing
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
