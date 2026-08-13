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
        zombies.emplace_back(std::move(zombie));
    }
}

void ZombieManager :: update(float dt) {
    for(auto& zombie : zombies) {
        if(zombie -> isFullyDead()) continue;

        if(gameplayMediator && !zombie->isDying()){
            zombie -> updateCombat(dt, *gameplayMediator);
        }

        zombie -> updateTime(dt);
    }

    zombies.erase(
        std::remove_if(
            zombies.begin(),
            zombies.end(),
            [](const std::unique_ptr<Zombie>& zombie) {
                return zombie->isFullyDead();
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

bool ZombieManager :: empty(void) const{
    return zombies.empty();
}
bool ZombieManager :: hasZombieReachedHouse(float houseBoundX) const{
    for(const auto&zombie : zombies){
        if(!zombie) continue;


        const ZombieState state = zombie -> getState();
        if(state == ZombieState :: DEAD || state == ZombieState :: DYING) continue;

        const Rectangle hitbox = zombie -> getHitbox();
        if(hitbox.x <= houseBoundX) return true;
    }
    return false;
}
