#ifndef ZOMBIE_MANAGER_HPP
#define ZOMBIE_MANAGER_HPP

#include "Gameplay/Zombies/Zombie.hpp"

class ZombieManager {
private:
    std::vector<std::unique_ptr<Zombie>> zombies;

public:
    void addZombie(std::unique_ptr<Zombie> zombie);
    void update(float dt);
    void draw() const;
    bool hasZombieInArea(Rectangle area) const;
    std :: vector<std::unique_ptr<Zombie>>& getZombies();
};

#endif
