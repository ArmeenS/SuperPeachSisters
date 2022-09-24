#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

#include <sstream>
#include <iomanip>

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath)
{
    curlevel = 1;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

Peach* StudentWorld::getPlayer() const { //returns pointer to Peach
    return player;
}

int StudentWorld::sumActors() const { //returns number of actors
    return actors.size();
}

Actor* StudentWorld::getActor(int pos) const {
    return actors[pos]; //returns pointer to the actor at specified posiiton
}

void StudentWorld::addActor(Actor* a) {
    actors.push_back(a); //adds an actor to the vector of pointers to actors
}

//void StudentWorld::deleteActor(Actor* a) {
//    vector<Actor*>::iterator it = actors.begin();
//    for (int i = 0; i < actors.size(); i++) {
//        if (actors[i] == a) {
//            delete actors[i];
//            actors.erase(it);
//            return;
//        }
//        it++;
//    }
//}

//int StudentWorld::getLevel() { return curlevel; } //returns curlevel
void StudentWorld::incLevel(int a) { curlevel = a; } //changes curlevel value

int StudentWorld::init()
{
    //cout<<getLevel()<<endl;
    current = new Level(assetPath());
    ostringstream oss;
    if (getLevel() > 0 && getLevel() < 100)
        oss << "level0" << getLevel() << ".txt";
    else
        oss << "level" << getLevel() << ".txt";
    string file = oss.str(); //gets file text
    Level::LoadResult result = current->loadLevel(file); //loads file
    if (result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format){
        //cout<<"bad"<<endl;
        return GWSTATUS_LEVEL_ERROR; //if file is in bad format or cannot be found
    }
    Level::GridEntry ge;
    int dir;
    for (int i = 0; i < 32; i++) { //load all actors to the screen
        for (int j = 0; j < 32; j++) {
            ge = current->getContentsOf(i, j);
            switch (ge) {
            case Level::empty:
                break;
            case Level::peach:
                player = new Peach(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT);
                break;
            case Level::block:
                actors.push_back(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 0));
                break;
            case Level::pipe:
                actors.push_back(new Pipe(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                break;
            case Level::flag:
                actors.push_back(new Flag(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                break;
            case Level::mario:
                actors.push_back(new Mario(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                break;
            case Level::mushroom_goodie_block:
                actors.push_back(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 1));
                break;
            case Level::flower_goodie_block:
                actors.push_back(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 2));
                break;
            case Level::star_goodie_block:
                actors.push_back(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 3));
                break;
            case Level::goomba:
                if (randInt(0, 1) == 0)
                    dir = 0;
                else
                    dir = 180;
                actors.push_back(new Goomba(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, dir));
                break;
            case Level::koopa:
                if (randInt(0, 1) == 0)
                    dir = 0;
                else
                    dir = 180;
                actors.push_back(new Koopa(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, dir));
                break;
            case Level::piranha:
                if (randInt(0, 1) == 0)
                    dir = 0;
                else
                    dir = 180;
                actors.push_back(new Piranha(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, dir));
                break;
            }
        }
    }
    curlevel = getLevel();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //cout<<getLevel()<<endl;
    player->doSomething(); //all actors can doSomething in a tick
    //int level = getLevel();
    for (int i = 0; i < actors.size(); i++) {
        actors[i]->doSomething();
        if (player->isDead()) { //if Peach died
            playSound(SOUND_PLAYER_DIE);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        else if (curlevel == -1) { //if Peach touched mario
            playSound(SOUND_GAME_OVER);
            return GWSTATUS_PLAYER_WON;
        }
        else if (curlevel != getLevel()) { //if Peach touched a flag
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    vector<Actor*>::iterator it = actors.begin(); //deletes all dead actors
    int i = 0;
    while (it != actors.end()) {
        if ((*it)->isDead()) {
            if ((*it)->getKoopa())
                actors.push_back(new Shell(this, (*it)->getX(), (*it)->getY(), (*it)->getDirection()));
            delete* it;
            actors.erase(it);
            it = actors.begin() + i;
        }
        else {
            i++;
            it++;
        }
    }

    //update game text
    ostringstream oss;
    oss.fill('0');
    oss << "Lives: " << getLives() << "  " << "Level: " << setw(2) << getLevel() << "  " << "Points: " << setw(6) << getScore();
    if(player->isStar())
        oss << " StarPower!";
    if(player->isFlower())
        oss << " ShootPower!";
    if(player->isMushroom())
        oss << " JumpPower!";
    text = oss.str();
    setGameStatText(text);

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() //deletes all dynamically allocated memory
{
    delete current;
    delete player;
    //vector<Actor*>::iterator it = actors.begin();
//    int k = 0;
//    while (it != actors.end()) {
//        delete* it;
//        actors.erase(it);
//        k++;
//        it = actors.begin() + k;
//    }
    vector<Actor*>::iterator it = actors.begin(); //deletes all dead actors
    int i = 0;
    while (it != actors.end()) {
        delete* it;
        actors.erase(it);
        it = actors.begin() + i;
    }
}
