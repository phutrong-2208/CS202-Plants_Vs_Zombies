#include "Gameplay/Plants/DefensivePlants/MagnetShroom.hpp"
PlantType MagnetShroom::getType() { return MAGNETSHROOM; }

void MagnetShroom::performAction(IGameplayMediator* mediator) {
    if (!mediator || isOnCooldown() || !plantData) return;
    
    Rectangle area = bounds;
    area.x -= 200.0f;
    area.width += 400.0f;
    area.y -= 200.0f;
    area.height += 400.0f;

    if (mediator->stripArmorInArea(area)) {
        triggerActionAnimation();
        cooldownTimer = plantData->getCooldown();
    }
}
