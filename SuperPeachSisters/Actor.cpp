#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* sworld, int imageID, int startX, int startY, int dir, int depth, double size)
    : GraphObject(imageID, startX, startY, dir, depth, size)
{
    world = sworld;
    status = false;
    damageable = false;
    blockable = false;
    iskoopa = false;
}

//i dont like these 2 functions
bool Actor::doesOverlap(int x1, int y1, Actor *a) const {
    //gets top left corner and bottom right corner of both rectangles
    int l1x = x1;
    int l1y = y1 + SPRITE_HEIGHT;
    int r1x = x1 + SPRITE_WIDTH;
    int r1y = y1;

    int l2x = a->getX();
    int l2y = a->getY() + SPRITE_HEIGHT;
    int r2x = a->getX() + SPRITE_WIDTH;
    int r2y = a->getY();
    
    //checks to see if x values fall within or y values fall within each other
    if (l1x >= r2x || l2x >= r1x) //x value of far left side of first rectangle is greater than the far right side of the other rectange -> cannot overlap
        //or vice verse for the other rectangle
        return false; //cannot overlap
    if (r1y >= l2y || r2y >= l1y) //y value of bottom of first rectangle is greater than the y value of the top of the other rectangle
        //or vice versa for the other rectangle
        return false; //cannot overlap
    return true; //they overlap
}

bool Actor::doesCollide(int x, int y) const {
    for (int i = 0; i < getWorld()->sumActors(); i++) { //loops through all actors
        if (getWorld()->getActor(i) != this && getWorld()->getActor(i)->isBlockable() && doesOverlap(x, y, getWorld()->getActor(i))) { //if it overlaps and it is a "blocking" actor, return true
            return true;
        }
    }
    return false;
}

bool Actor::doesDamage(int x, int y) const {
    for (int i = 0; i < getWorld()->sumActors(); i++) { //loops through all actors
        if (getWorld()->getActor(i) != this && getWorld()->getActor(i)->isDamageable() && doesOverlap(x, y, getWorld()->getActor(i))) { //if it overlaps and it can damage the actor it is touching
            getWorld()->getActor(i)->setDead(); //kill the touched actor
            return true;
        }
    }
    return false;
}

Peach::Peach(StudentWorld* sworld, int startX, int startY)
    : Actor(sworld, IID_PEACH, startX, startY, 0, 0, 1.0)
{
    health = 1; //default Peach settings
    mushroom = false;
    remaining_jump_distance = 0;
    flower = false;
    recharge = 0;
    star = false;
    invincible = 0;

    setDamageable();
}

void Peach::doSomething() {
    //if Peach is dead, return immediately
    if (isDead()) {
         return;
     }

    //invincibility (star)
    if (invincible > 0)
        invincible--;
    if (invincible == 0)
        star = false;

    //invincibility (damaged)
    if (tempinvincible > 0)
        tempinvincible--;
    if (tempinvincible == 0)
        tempinv = false;

    //recharge timer
    if (recharge > 0)
        recharge--;

    //overlaps
    for (int i = 0; i < getWorld()->sumActors(); i++) {
        if (doesOverlap(getX(), getY(), getWorld()->getActor(i)))
            getWorld()->getActor(i)->bonk(); //loop through all actors and if they overlap with Peach, bonk them
    }

    //jumping mechanic
    if (remaining_jump_distance > 0) {
        if (doesCollide(getX(), getY() + 4)) { //if she is blocked from above
            remaining_jump_distance = 0; //do not jump up any further
            for (int i = 0; i < getWorld()->sumActors(); i++) {
                if (doesOverlap(getX(), getY() + 4, getWorld()->getActor(i))) {
                    getWorld()->getActor(i)->bonk(); //if she overlaps an Actor at that position, bonk them
                    break;
                }
            }
        }
        else {
            moveTo(getX(), getY() + 4); //nothing blocks above, jump up
            remaining_jump_distance--;
        }
    }
    else {
        bool block = false;
        for (int i = 0; i <= 3; i++) {
            if (doesCollide(getX(), getY() - i)) {
                block = true; //Checks if Peach is supported from below by 3 pixels
                break;
            }
        }
        if (!block) //if she is not supported underneath, move her down
            moveTo(getX(), getY() - 4);
    }

    //input
    int input;
    getWorld()->getKey(input);
    switch (input) {
    case KEY_PRESS_LEFT:
        if (!doesCollide(getX() - 4, getY())) { //if she is not blocked by an Actor from the left
            setDirection(180);
            moveTo(getX() - 4, getY()); //move her left
        }
        else
            getWorld()->playSound(SOUND_PLAYER_BONK);
        break;
    case KEY_PRESS_RIGHT:
        if (!doesCollide(getX() + 4, getY())) { //if she is not blocked by an Actor from the right
            setDirection(0);
            moveTo(getX() + 4, getY()); //move her to the right
        }
        else
            getWorld()->playSound(SOUND_PLAYER_BONK);
        break;
    case KEY_PRESS_UP:
        if (doesCollide(getX(), getY() - 1)) { //if she is supported underneath
            if (isMushroom()) { remaining_jump_distance = 12; }
            else { remaining_jump_distance = 8; }
            getWorld()->playSound(SOUND_PLAYER_JUMP); //she is able to jump
        }
        break;
    case KEY_PRESS_SPACE:
        if (isFlower() && recharge == 0) { //if she has the Shoot Power and can shoot (recharge timer is 0)
            getWorld()->playSound(SOUND_PLAYER_FIRE);
            recharge = 8;
            if (getDirection() == 0) //she can shoot a fireball in the direction she is facing
                getWorld()->addActor(new PeachFire(getWorld(), getX() + 4, getY(), getDirection()));
            else
                getWorld()->addActor(new PeachFire(getWorld(), getX() - 4, getY(), getDirection()));
        }
        break;
    }
}

void Peach::bonk() {
    if (star || tempinv) //she is invincible, no reaction to being bonked
        return;
    else {
        health--;
        if (health == 1) //she is alive but hurt
            getWorld()->playSound(SOUND_PLAYER_HURT);
        else //health == 0, therefore she is dead
            setDead();
        mushroom = false; //if she is still alive, all powerups are off
        flower = false;
        tempinv = true; //however temp invincibility is turned on
        tempinvincible = 10;
    }
}

Ground::Ground(StudentWorld* sworld, int imageID, int startX, int startY)
    : Actor(sworld, imageID, startX, startY, 0, 2, 1.0)
{
    setBlockable(); //blocks other Actors from moving on top of it
}

void Ground::doSomething() {} //these Actors do not move


Block::Block(StudentWorld* sworld, int startX, int startY, int powerup)
    : Ground(sworld, IID_BLOCK, startX, startY)
{
    contains = powerup; //blocks can contain powerups
}

void Block::bonk() {
    switch (contains) {
    case 1: //contains = 1 for mushroom
        getWorld()->playSound(SOUND_POWERUP_APPEARS);
        getWorld()->addActor(new Mushroom(getWorld(), getX(), getY() + 8));
        break; //create mushroom
    case 2: //contains = 2 for flower
        getWorld()->playSound(SOUND_POWERUP_APPEARS);
        getWorld()->addActor(new Flower(getWorld(), getX(), getY() + 8));
        break; //create flower
    case 3: //contains = 3 for star
        getWorld()->playSound(SOUND_POWERUP_APPEARS);
        getWorld()->addActor(new Star(getWorld(), getX(), getY() + 8));
        break; //create star
    default: //contains = 0 for empty
        getWorld()->playSound(SOUND_PLAYER_BONK);
    }
    contains = 0; //block is empty at the end
}

Pipe::Pipe(StudentWorld* sworld, int startX, int startY)
    : Ground(sworld, IID_PIPE, startX, startY)
{
}

void Pipe::bonk(){} //pipes do not react to being bonked

End::End(StudentWorld* sworld, int imageID, int startX, int startY)
    : Actor(sworld, imageID, startX, startY, 0, 1, 1.0)
{
}

void End::doSomething() {
    if (isDead()) //do nothing if the actor is dead
        return;
    if (doesOverlap(getX(), getY(), getWorld()->getPlayer())) { //if the Flag/Mario overlaps with Peach, increase the score and go to the next level/end the game
        getWorld()->increaseScore(1000);
        setDead();
        complete();
    }
}

void End::bonk() {} //End actors do not do anything in bonked since the doSomething takes care of what happens when it is touched by Peach

Flag::Flag(StudentWorld* sworld, int startX, int startY)
    : End(sworld, IID_FLAG, startX, startY)
{
}

void Flag::complete() {
    int level = getWorld()->getLevel();
    getWorld()->incLevel(level+1); //increases the curlevel variable by 1
}

Mario::Mario(StudentWorld* sworld, int startX, int startY)
    : End(sworld, IID_MARIO, startX, startY)
{
}

void Mario::complete() {
    getWorld()->incLevel(-1); //changes the curlevel variable to -1
}

Powerup::Powerup(StudentWorld* sworld, int imageID, int startX, int startY)
    : Actor(sworld, imageID, startX, startY, 0, 1, 1)
{
}

void Powerup::doSomething() {
    if (doesOverlap(getX(), getY(), getWorld()->getPlayer())) {
        setPower(); //sets the respective powerup to Peach
        setDead();
        getWorld()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }

    if (!doesCollide(getX(), getY() - 1)) //if not supported underneath, move it down
        moveTo(getX(), getY() - 2);

    if (getDirection() == 0) { //if something blocks it from the way it goes, change its direction, else, move it in the direction it is originally facing
        if (doesCollide(getX() + 2, getY())) {
            setDirection(180);
            return;
        }
        moveTo(getX()+2, getY());
    }
    else {
        if (doesCollide(getX() - 2, getY())) {
            setDirection(0);
            return;
        }
        moveTo(getX() - 2, getY());
    }
}

void Powerup::bonk() {} //powerups do nothing when bonked

Mushroom::Mushroom(StudentWorld* sworld, int startX, int startY)
    : Powerup(sworld, IID_MUSHROOM, startX, startY)
{}

void Mushroom::setPower() {
    getWorld()->increaseScore(75); //happens when overlapped with Peach
    getWorld()->getPlayer()->setMushroom();
    getWorld()->getPlayer()->incHealth(); //set hitpoints to 2
}

Flower::Flower(StudentWorld* sworld, int startX, int startY)
    : Powerup(sworld, IID_FLOWER, startX, startY)
{}

void Flower::setPower() {
    getWorld()->increaseScore(50); //happens when overlapped with Peach
    getWorld()->getPlayer()->setFlower();
    getWorld()->getPlayer()->incHealth(); //set hitpoints to 2
}

Star::Star(StudentWorld* sworld, int startX, int startY)
    : Powerup(sworld, IID_STAR, startX, startY)
{}

void Star::setPower() {
    getWorld()->increaseScore(100); //happens when overlapped with Peach
    getWorld()->getPlayer()->setStar();
}

Projectile::Projectile(StudentWorld* sworld, int imageID, int startX, int startY, int dir)
    : Actor(sworld, imageID, startX, startY, dir, 1, 1.0)
{
    //setDamageable(); //?
}

void Projectile::doSomething() {
    touch(); //special function if projectile touches Peach/actor

    if (!doesCollide(getX(), getY() - 1)) { //if not supported underneath, move it down
        moveTo(getX(), getY() - 2);
    }
    if (getDirection() == 0) { //if it collides, kill the projectile, if not move it to the respective location
        if (doesCollide(getX() + 2, getY())) {
            setDead();
            return;
        }
        else
            moveTo(getX() + 2, getY());
    }
    else
        if (doesCollide(getX() - 2, getY())) {
            setDead();
            return;
        }
        else
            moveTo(getX() - 2, getY());
}

void Projectile::touch() { //this is common function for Peach's Fireballs and Shells since they do NOT damage Peach but damage damageable actors that overlap them
    if (doesDamage(getX(), getY())) {
        getWorld()->increaseScore(100);
        //damage ACTOR
        setDead();
        return;
    }
}

void Projectile::bonk() {} //projectiles do nothing when bokned

PiranhaFire::PiranhaFire(StudentWorld* sworld, int startX, int startY, int dir)
    : Projectile(sworld, IID_PIRANHA_FIRE, startX, startY, dir)
{
}

void PiranhaFire::touch() { //Piranha fireballs can damage Peach (calls bonk if overlaps)
    if (doesOverlap(getX(), getY(), getWorld()->getPlayer())) {
        getWorld()->getPlayer()->bonk(); //damaged
        setDead();
        return;
    }
}

PeachFire::PeachFire(StudentWorld* sworld, int startX, int startY, int dir)
    : Projectile(sworld, IID_PEACH_FIRE, startX, startY, dir)
{
}

Shell::Shell(StudentWorld* sworld, int startX, int startY, int dir)
    : Projectile(sworld, IID_SHELL, startX, startY, dir)
{
}

//void Shell::touch(){}

Enemies::Enemies(StudentWorld* sworld, int imageID, int startX, int startY, int dir)
    : Actor(sworld, imageID, startX, startY, dir, 0, 1.0)
{
    setDamageable(); //enemies are damageable
}

void Enemies::doSomething() {
    if(isDead())
        return;
    if (doesOverlap(getX(), getY(), getWorld()->getPlayer())) { //if it overlaps with peach, bonk peach
        getWorld()->getPlayer()->bonk();
        return;
    }
    movement(); //goomba and koopas share the same movement however piranhas have different movement
}

void Enemies::movement() { //movement common for goomba and koopa class
    if (getDirection() == 0) {
        if (doesCollide(getX() + 1, getY())) //if its blocked, change direction
            setDirection(180);
        else {
            if (!doesCollide(getX() + SPRITE_WIDTH, getY() - 1)) //if theres a "hole", change directions
                setDirection(180);
        }
    }
    else {
        if (doesCollide(getX() - 1, getY())) //if its blocked, change direction
            setDirection(0);
        else {
            if (!doesCollide(getX() - SPRITE_WIDTH, getY() - 1)) //if theres a "hole", change directions
                setDirection(0);
        }
    }
    if (getDirection() == 0) { //if no actor/hole blocks the goomba/koopa, move it to the desired location
        if (doesCollide(getX() + 1, getY()))
            return;
        else
            moveTo(getX() + 1, getY());
    }
    else {
        if (doesCollide(getX() - 1, getY()))
            return;
        else
            moveTo(getX() - 1, getY());
    }
}

void Enemies::bonk() {
    //ignore if not by peach
    if (getWorld()->getPlayer()->isStar()) { //if player has the star power and bonks an enemy
        getWorld()->playSound(SOUND_PLAYER_KICK); //kick/kill the enemy
        getWorld()->increaseScore(100);
        setDead();
    }
}

Goomba::Goomba(StudentWorld* sworld, int startX, int startY, int dir)
    :Enemies(sworld, IID_GOOMBA, startX, startY, dir)
{}

Koopa::Koopa(StudentWorld* sworld, int startX, int startY, int dir)
    : Enemies(sworld, IID_KOOPA, startX, startY, dir)
{
    setKoopa(); //it is a koopa
}

Piranha::Piranha(StudentWorld* sworld, int startX, int startY, int dir)
    : Enemies(sworld, IID_PIRANHA, startX, startY, dir)
{
    delay = 0;
}

void Piranha::movement(){
    increaseAnimationNumber();
    //detection if Peach is not on same level as the Piranha
    if(!((getWorld()->getPlayer()->getY() > getY()-SPRITE_HEIGHT*0.5/2.0 && getWorld()->getPlayer()->getY() < getY()+SPRITE_HEIGHT*-.5/2.0) || (getWorld()->getPlayer()->getY()+SPRITE_HEIGHT > getY()-SPRITE_HEIGHT*0.5/2.0&&getWorld()->getPlayer()->getY()+SPRITE_HEIGHT<getY()+SPRITE_HEIGHT*0.5/2.0)))
        return;
    //face direction Peach is located
    int d = 0;
    if(getWorld()->getPlayer()->getY() < getX())
        d = 180;
    setDirection(d);
    //shoot delay
    if(delay > 0){
        delay--;
        return;
    }
    //if in range of Piranha, shoot a fireball
    if((d == 0 && getWorld()->getPlayer()->getX() - getX() < 8*SPRITE_WIDTH)||(d == 180 && getX()-getWorld()->getPlayer()->getX() < 8*SPRITE_WIDTH)){
        getWorld()->addActor(new PiranhaFire(getWorld(), getX(), getY(), getDirection()));
        getWorld()->playSound(SOUND_PIRANHA_FIRE);
        delay = 40;
    }
}
