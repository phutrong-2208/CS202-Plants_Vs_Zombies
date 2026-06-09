#ifndef PLANT_HPP
#define PLANT_HPP

#include "raylib.h"

class Plant {
protected:
    Vector2 position;
    int health;
    int sunCost;

public:
    Plant(Vector2 pos, int hp, int cost);
    virtual ~Plant() = default;

    void receiveDamage(int damage);

    bool isDead() const;
    Vector2 getPosition() const;
    int getHealth() const;
    int getCost() const;
};

#endif
