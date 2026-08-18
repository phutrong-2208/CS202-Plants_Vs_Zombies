#include "Gameplay/Plants/ShooterPlants/ThreePeater.hpp"
#include "Gameplay/Zombies/Zombie.hpp"

PlantType ThreePeater::getType() { return THREEPEATER; }

void ThreePeater::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    if (plantData && plantData->getCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        float cellHeight = bounds.height; 
        
        Rectangle sensorMiddle = {bounds.x, bounds.y, 1000.0f, bounds.height};
        Rectangle sensorTop = {bounds.x, bounds.y - cellHeight, 1000.0f, bounds.height};
        Rectangle sensorBottom = {bounds.x, bounds.y + cellHeight, 1000.0f, bounds.height};
        
        bool hasTarget = mediator->hasZombieInArea(sensorMiddle) || 
                         mediator->hasZombieInArea(sensorTop) || 
                         mediator->hasZombieInArea(sensorBottom);
                         
        if (hasTarget) {
            Vector2 spawnPos = getProjectileSpawnPosition();
            
            // Spawn 3 projectiles
            if (mediator->hasZombieInArea(sensorTop)) {
                mediator->addProjectile(getType(), {spawnPos.x, spawnPos.y - cellHeight}, getDamage());
                animation.playClipLayer("shooting1", 0);
            }
            if (mediator->hasZombieInArea(sensorMiddle)) {
                mediator->addProjectile(getType(), spawnPos, getDamage());
                animation.playClipLayer("shooting2", 1);
            }
            if (mediator->hasZombieInArea(sensorBottom)) {
                mediator->addProjectile(getType(), {spawnPos.x, spawnPos.y + cellHeight}, getDamage());
                animation.playClipLayer("shooting3", 2);
            }
            
            mediator->playSound("THROW", 0.6f);
            resetCooldown();
        }
    }
}
