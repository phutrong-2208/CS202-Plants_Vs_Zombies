#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include "Gameplay/Animation/ReanimInstance.hpp"

class Zombie{
protected:
    int health; 
    float speed;
    int attackDamage;
    bool attacking;
    Rectangle hitbox;
    ReanimInstance animation;
public:
    Zombie(Rectangle hitbox, int hp, float moveSpeed, int damage);
    virtual ~Zombie() = default;

    virtual void updateTime(float dt);
    void setReanimInstance(ReanimInstance anim);
    void draw(void);

    void receiveDamage(int damage);
    bool isDead(void) const;

    int getHealth(void) const;
    float getSpeed(void) const;
    int getAttackDamage(void) const;
    Rectangle getHitbox(void) const;

    void setHitbox(Rectangle newHitbox);
    void setAttacking(bool isAttacking);
    bool isAttacking(void) const;

};
#endif
