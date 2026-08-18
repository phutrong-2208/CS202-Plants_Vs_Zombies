#include "Gameplay/Zombies/ZombieManager.hpp"

void ZombieManager::setMediator(IGameplayMediator* mediator) {
    gameplayMediator = mediator;
}

ZombieDeathHandler& ZombieManager::getDeathHandler() {
    return deathHandler;
}

void ZombieManager :: addZombie(std::unique_ptr<Zombie> zombie) {
    if(zombie) {
        zombie->setDeathHandler(&deathHandler);
        pendingZombies.emplace_back(std::move(zombie));
    }
}

void ZombieManager :: update(float dt) {
    // Flush any zombies added prior to this frame's update
    if (!pendingZombies.empty()) {
        for (auto& z : pendingZombies) {
            zombies.emplace_back(std::move(z));
        }
        pendingZombies.clear();
    }

    const size_t count = zombies.size();
    for(size_t i = 0; i < count; ++i) {
        auto& zombie = zombies[i];
        if(!zombie || zombie -> isFullyDead()) continue;

        if(gameplayMediator && !zombie->isDying()){
            zombie -> updateCombat(dt, *gameplayMediator);
        }

        if (zombie && !zombie->isFullyDead()) {
            zombie -> updateTime(dt);
        }
    }

    // Flush any zombies spawned during combat updates (e.g. Thrown Imps, Backup Dancers)
    if (!pendingZombies.empty()) {
        for (auto& z : pendingZombies) {
            zombies.emplace_back(std::move(z));
        }
        pendingZombies.clear();
    }

    zombies.erase(
        std::remove_if(
            zombies.begin(),
            zombies.end(),
            [](const std::unique_ptr<Zombie>& zombie) {
                return !zombie || zombie->isFullyDead();
            }
        ),
        zombies.end()
    );
}

void ZombieManager :: draw() const {
    for(const auto& zombie : zombies) {
        if(!zombie->isFullyDead()) zombie->draw();
    }
}

Zombie* ZombieManager::getZombiePriority(Rectangle area) {
    Zombie* target = nullptr;
    float distance = 1e6;

    for (auto& zombie : zombies) {
        if (zombie -> isDead()) continue;

        Rectangle hitbox = zombie -> getHitbox();
        if (!CheckCollisionRecs(area, hitbox)) continue;

        // Heuristic algorithm to find the nearest zombie to be hit.
        // This assumes that the bullets are slow enough to not jump through the zombie hitboxes.
        Vector2 areaCenter = {area.x + area.width * 0.5f, area.y + area.height * 0.5f};
        Vector2 hitboxCenter = {hitbox.x + hitbox.width * 0.5f, hitbox.y + hitbox.height * 0.5f};

        float localDist = Vector2Distance(areaCenter, hitboxCenter);
        if (distance > localDist) {
            distance = localDist;
            target = zombie.get();
        }
    }

    return target;
}

Zombie* ZombieManager::getZombieWithArmor(Rectangle area) {
    float distance = 1e9;
    Zombie* target = nullptr;

    for (auto& zombie : zombies) {
        if (zombie->isDead() || zombie->getArmorHealth() <= 0.0f) continue;
        
        // Also check if it's metallic? Buckethead and Football are metallic.
        // Screen door is not metallic? Actually screen door is metallic in PVZ!
        // We'll just assume all armor is metallic for now.

        Rectangle hitbox = zombie->getHitbox();
        if (!CheckCollisionRecs(area, hitbox)) continue;

        Vector2 areaCenter = {area.x + area.width * 0.5f, area.y + area.height * 0.5f};
        Vector2 hitboxCenter = {hitbox.x + hitbox.width * 0.5f, hitbox.y + hitbox.height * 0.5f};

        float localDist = Vector2Distance(areaCenter, hitboxCenter);
        if (distance > localDist) {
            distance = localDist;
            target = zombie.get();
        }
    }
    return target;
}
bool ZombieManager::hasZombieInArea(Rectangle area, Zombie* exclude) const {
    for (auto& zombie : zombies) {
        if (!zombie->isDead() && zombie.get() != exclude) {
            Rectangle hitbox = zombie->getHitbox();
            if (CheckCollisionRecs(area, hitbox)) return true;
        }
    }
    return false;
}


std :: vector<std :: unique_ptr<Zombie>>& ZombieManager :: getZombies() {
    return zombies;
}

const std :: vector<std :: unique_ptr<Zombie>>& ZombieManager :: getZombies() const {
    return zombies;
}

bool ZombieManager :: empty(void) const{
    return zombies.empty();
}
bool ZombieManager :: hasZombieReachedHouse(float houseBoundX) const{
    for(const auto& zombie : zombies){
        if(!zombie) continue;

        if(zombie->isDead() || zombie->isDying() || zombie->isFullyDead() || zombie->getHealth() <= 0.0f) continue;
        if(zombie->getIsHypnotized() || zombie->isSwallowed()) continue;

        const Rectangle hitbox = zombie -> getHitbox();
        if(hitbox.x <= houseBoundX) return true;
    }
    return false;
}
