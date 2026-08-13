#ifndef ZOMBIE_MANAGER_HPP
#define ZOMBIE_MANAGER_HPP

#include <Gameplay/IGameplayMediator.hpp>
#include "Gameplay/Zombies/Zombie.hpp"
#include "Gameplay/Zombies/ZombieDeathHandler.hpp"

class ZombieManager {
private:
    std::vector<std::unique_ptr<Zombie>> zombies;
    IGameplayMediator* gameplayMediator = nullptr;
    ZombieDeathHandler deathHandler;
public:
    void setMediator(IGameplayMediator* mediator);
    ZombieDeathHandler& getDeathHandler();
    
    void addZombie(std::unique_ptr<Zombie> zombie);
    void update(float dt);
    void draw() const;

    Zombie* getShotFirst(Rectangle area);
    bool hasZombieInArea(Rectangle area) const;
    std :: vector<std::unique_ptr<Zombie>>& getZombies();
    bool empty(void) const;
    bool hasZombieReachedHouse(float houseBoundX) const;
};

#endif
