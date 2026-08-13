#include "Gameplay/Plants/ShooterPlants/SplitPea.hpp"
PlantType SplitPea::getType() { return SPLITPEA; }

void SplitPea::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    if (plantData && plantData->getCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        float range = 1000.0f; 
        
        Rectangle sensorForward = {bounds.x, bounds.y, range, bounds.height};
        Rectangle sensorBackward = {bounds.x - range, bounds.y, range, bounds.height};
        
        bool hasTargetForward = mediator->hasZombieInArea(sensorForward);
        bool hasTargetBackward = mediator->hasZombieInArea(sensorBackward);
                         
        if (hasTargetForward || hasTargetBackward) {
            Vector2 spawnPos = getProjectileSpawnPosition();
            
            if (hasTargetForward) {
                mediator->addProjectile(getType(), spawnPos, getDamage(), false);
                animation.playClipLayer("shooting", 0);
            }
            if (hasTargetBackward) {
                // Standard PvZ shoots 2 backwards. We spawn them slightly spaced apart.
                mediator->addProjectile(getType(), spawnPos, getDamage(), true);
                mediator->addProjectile(getType(), {spawnPos.x + 30.0f, spawnPos.y}, getDamage(), true); // spaced out visually
                animation.playClipLayer("splitpea_shooting", 1);
            }
            
            resetCooldown();
        }
    }
}
