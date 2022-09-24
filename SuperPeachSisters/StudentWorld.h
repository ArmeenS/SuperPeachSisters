#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

#include "Actor.h"
#include <vector>
using namespace std;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Peach* getPlayer() const;
    int sumActors() const;
    Actor* getActor(int pos) const;
    void addActor(Actor* a);
    //void deleteActor(Actor* a);

    //int getLevel();
    void incLevel(int a);

private:
    int curlevel;
    Level* current;
    Peach* player;
    vector<Actor*> actors;
    string text;
};

#endif // STUDENTWORLD_H_
