#include "Gameplay/Plants/Plant.hpp"

Plant :: Plant(int hp, int cost){
    health = hp;
    sunCost = cost;
}

void Plant::updateTime(float deltaSeconds) {
    animation.updateTime(deltaSeconds);
}
void Plant::draw(Rectangle hitbox) {
    animation.draw(hitbox);   
}
void Plant::setReanimInstance(ReanimInstance anim) {
    animation = anim;
}


void Plant :: receiveDamage(int damage){
    health -= damage;
    if(health < 0) health = 0;
}

bool Plant :: isDead(void) const {
    if(health == 0) return 1;
    return 0;
}

int Plant :: getHealth(void) const {
    return health;
}

int Plant :: getCost(void) const {
    return sunCost;
}



