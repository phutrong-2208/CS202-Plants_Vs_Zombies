#include "Gameplay/Zombies/Zombie.hpp"

Zombie :: Zombie(Rectangle hitbox, int hp, float moveSpeed, int damage)
: health(hp), speed(moveSpeed), attackDamage(damage), attacking(false), hitbox(hitbox)
{}

void Zombie :: updateTime(float dt) {
    animation.updateTime(dt);

    if (!attacking) {
        hitbox.x -= speed * dt;
    }
}

void Zombie :: setReanimInstance(ReanimInstance anim) {
    animation = anim;
}

void Zombie :: draw(void) {
    animation.draw(hitbox);
}

void Zombie :: receiveDamage(int damage){
    health -= damage;
    if(health < 0) health = 0;
}

bool Zombie :: isDead(void) const {
    if(health == 0) return 1;
    return 0;
}

int Zombie :: getHealth(void) const {
    return health;
}

float Zombie :: getSpeed(void) const {
    return speed;
}

int Zombie :: getAttackDamage(void) const {
    return attackDamage;
}

Rectangle Zombie :: getHitbox(void) const {
    return hitbox;
}

void Zombie :: setHitbox(Rectangle newHitbox) {
    hitbox = newHitbox;
}

void Zombie :: setAttacking(bool isAttacking) {
    attacking = isAttacking;
}

bool Zombie :: isAttacking(void) const {
    return attacking;
}
