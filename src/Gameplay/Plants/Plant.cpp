#include "Gameplay/Plants/Plant.hpp"

Plant :: Plant(Vector2 pos, int hp, int cost){
    position = pos;
    health = hp;
    sunCost = cost;
}


void Plant :: receiveDamage(int damage){
    health -= damage;
    if(health < 0) health = 0;
}

bool Plant :: isDead(void) const {
    if(health == 0) return 1;
    return 0;
}

Vector2 Plant :: getPosition(void) const {
    return position;
}

int Plant :: getHealth(void) const {
    return health;
}

int Plant :: getCost(void) const {
    return sunCost;
}



