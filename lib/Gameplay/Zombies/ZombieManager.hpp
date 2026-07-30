#ifndef ZOMBIE_MANAGER_HPP
#define ZOMBIE_MANAGER_HPP

#include <Gameplay/IGameplayMediator.hpp>
#include "Gameplay/Zombies/Zombie.hpp"

class ZombieManager {
private:
    std::vector<std::unique_ptr<Zombie>> zombies;
    IGameplayMediator* gameplayMediator = nullptr;
public:
    void setMediator(IGameplayMediator* mediator);
    
    void addZombie(std::unique_ptr<Zombie> zombie);
    void update(float dt);
    void draw() const;

    Zombie* getShotFirst(Rectangle area);
    bool hasZombieInArea(Rectangle area) const;
    std :: vector<std::unique_ptr<Zombie>>& getZombies();
};

#endif
