#ifndef PLANT_HPP
#define PLANT_HPP

#include <Gameplay/Animation/ReanimInstance.hpp>

class Plant {
protected:
    int health;
    int sunCost;
    Rectangle hitbox;
    ReanimInstance animation;
public:
    Plant(Rectangle territory, int hp, int cost);
    void receiveDamage(int damage);

    void updateTime(float deltaSeconds);
    void setReanimInstance(ReanimInstance anim);
    void draw();
    
    bool isDead() const;
    int getHealth() const;
    int getCost() const;
};

#endif
