#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(StudentWorld* sworld, int imageID, int startX, int startY, int dir, int depth, double size);
    virtual void doSomething() = 0;
    virtual void bonk() = 0;
    StudentWorld* getWorld() const { return world; } //pointer to world the actors live in

    //set/get status variable which indicates if actor is alive or not
    void setDead() { status = true; }
    bool isDead() const { return status; }

    //set/get damageable variable which indicates if actor is damageable or not
    void setDamageable() { damageable = true; }
    bool isDamageable() const { return damageable; }

    //set/get blockable variable which indicates if actor can block other actors or not
    void setBlockable() { blockable = true; }
    bool isBlockable() { return blockable; }

    //set/get iskoopa variable which indicates if actor is a koopa or not
    void setKoopa() { iskoopa = true; }
    bool getKoopa() { return iskoopa; }

    bool doesOverlap(int x1, int y1, Actor *a) const; //checks if rectangles overlap
    bool doesCollide(int x, int y) const; //checks if each actor collides with peach
    bool doesDamage(int x, int y) const; //checks if actor damages the other actor

private:
    StudentWorld* world;
    bool status;
    bool damageable;
    bool blockable;
    bool iskoopa;
};


class Peach : public Actor {
public:
    Peach(StudentWorld* sworld, int startX, int startY);

    void doSomething();
    void bonk();

    //set/get health variable which indicates much hits Peach can take
    int getHealth() const { return health; }
    void incHealth() { health = 2; }

    //set/get flower variable which indicates if actor has Shoot Power or not
    bool isFlower() const { return flower; }
    void setFlower() { flower = true; }

    //set/get mushroom variable which indicates if actor has Jump Power or not
    bool isMushroom() { return mushroom; }
    void setMushroom() { mushroom = true; }

    //set/get star variable which indicates if actor has Star Power or not
    bool isStar() const { return star; }
    void setStar() {
        star = true;
        invincible = 150;
    }
private:
    int health;
    bool flower;
    int recharge;
    bool mushroom;
    int remaining_jump_distance;
    bool star;
    int invincible;
    bool tempinv;
    int tempinvincible;
};


class Ground : public Actor { //Superclass of BLOCK and PIPE
public:
    Ground(StudentWorld* sworld, int imageID, int startX, int startY);
    void doSomething();
    virtual void bonk() = 0;
};

class Block : public Ground {
public:
    Block(StudentWorld* sworld, int startX, int startY, int powerup);
    void bonk();
private:
    int contains; //0 for nothing, 1 for mushroom, 2 for flower, 3 for star
};

class Pipe : public Ground {
public:
    Pipe(StudentWorld* sworld, int startX, int startY);
    void bonk();
};


class End : public Actor { //Superclass of FLAG and MARIO
public:
    End(StudentWorld* sworld, int imageID, int startX, int startY);
    void doSomething();
    void bonk();
    virtual void complete() = 0;
};

class Flag : public End {
public:
    Flag(StudentWorld* sworld, int startX, int startY);
    void complete();
};

class Mario : public End {
public:
    Mario(StudentWorld* sworld, int startX, int startY);
    void complete();
};


class Powerup : public Actor { //Superclass of MUSHROOM, FLOWER, and STAR
public:
    Powerup(StudentWorld* sworld, int imageID, int startX, int startY);
    void doSomething();
    void bonk();

    virtual void setPower() = 0;
};

class Mushroom : public Powerup {
public:
    Mushroom(StudentWorld* sworld, int startX, int startY);
    void setPower();
};

class Flower : public Powerup {
public:
    Flower(StudentWorld* sworld, int startX, int startY);
    void setPower();
};

class Star : public Powerup {
public:
    Star(StudentWorld* sworld, int startX, int startY);
    void setPower();
};


class Projectile : public Actor { //Superclass of PIRANHAFIRE, PEACHFIRE, and SHELL
public:
    Projectile(StudentWorld* sworld, int imageID, int startX, int startY, int dir);
    void doSomething();
    void bonk();

    virtual void touch();
};

class PiranhaFire : public Projectile {
public:
    PiranhaFire(StudentWorld* sworld, int startX, int startY, int dir);
    void touch();
};

class PeachFire : public Projectile {
public:
    PeachFire(StudentWorld* sworld, int startX, int startY, int dir);
};

class Shell : public Projectile {
public:
    Shell(StudentWorld* sworld, int startX, int startY, int dir);
    //void touch();
};


class Enemies : public Actor { //Superclass of GOOMBA, KOOPA, and PIRANHA
public:
    Enemies(StudentWorld* sworld, int imageID, int startX, int startY, int dir);
    void doSomething();
    virtual void movement();
    void bonk();
};

class Goomba : public Enemies {
public:
    Goomba(StudentWorld* sworld, int startX, int startY, int dir);
};

class Koopa : public Enemies {
public:
    Koopa(StudentWorld* sworld, int startX, int startY, int dir);
};

class Piranha : public Enemies {
public:
    Piranha(StudentWorld* sworld, int startX, int startY, int dir);
    void movement();
private:
    int delay;
};

#endif // ACTOR_H_
