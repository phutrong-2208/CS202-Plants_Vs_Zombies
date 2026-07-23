#include "Gameplay/Zombies/ZombieManager.hpp"

void ZombieManager :: addZombie(std::unique_ptr<Zombie> zombie) {
    if(zombie) zombies.emplace_back(std::move(zombie));
}

void ZombieManager::update(float dt) {
    for(auto& zombie : zombies) {
        if(!zombie->isDead()) zombie->updateTime(dt);
    }

    zombies.erase(
        std::remove_if(
            zombies.begin(),
            zombies.end(),
            [](const std::unique_ptr<Zombie>& zombie) {
                return zombie->isDead();
            }
        ),
        zombies.end()
    );
}

void ZombieManager :: draw() const {
    for(const auto& zombie : zombies) {
        if(!zombie->isDead()) zombie->draw();
    }
}

bool ZombieManager :: hasZombieInArea(Rectangle area) const{
    for(auto& zombie : zombies) if(!zombie -> isDead()){
        Rectangle hitbox = zombie -> getHitbox();
        if(CheckCollisionRecs(area, hitbox)) return true;
    }
    return false;
}


std :: vector<std :: unique_ptr<Zombie>>& ZombieManager :: getZombies() {
    return zombies;
}
