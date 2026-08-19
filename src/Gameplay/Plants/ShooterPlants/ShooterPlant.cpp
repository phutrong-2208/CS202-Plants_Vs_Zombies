#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"

ShooterPlant::ShooterPlant() = default;

Vector2 ShooterPlant::getProjectileSpawnPosition() {
    if (getType() == CABBAGEPULT || getType() == MELONPULT || getType() == WINTERMELON || getType() == CORNPULT) {
        return {bounds.x + 80.0f, bounds.y - 10.0f}; // Higher up for lobbing
    }
    return {bounds.x + 108.0f, bounds.y + 45.0f};
}

void ShooterPlant::updateTime(float dt) {
    Plant::updateTime(dt);

    resetAnimTimer = std::max(0.0f, resetAnimTimer - dt);

    if (resetAnimTimer <= 0.0f && !readyToFire && fireDelayTimer <= 0.0f) {
        if (getType() == CABBAGEPULT) {
            animation.unhideTrack("Cabbagepult_cabbage");
        } else if (getType() == CORNPULT) {
            animation.unhideTrack("Cornpult_kernal");
            animation.unhideTrack("Cornpult_butter");
        } else if (getType() == MELONPULT || getType() == WINTERMELON) {
            animation.unhideTrack("Melonpult_mellon");
        }
    }

    if (fireDelayTimer > 0.0f) {
        fireDelayTimer -= dt;
        if (fireDelayTimer <= 0.0f) {
            readyToFire = true;
            fireDelayTimer = 0.0f;
        }
    }
}

void ShooterPlant::onActionAnimationFinished() {
    if (fireDelayTimer <= 0.0f) {
        readyToFire = true;
        fireDelayTimer = 0.0f;
    }
}

void ShooterPlant::performAction(IGameplayMediator* mediator) {
    if (plantData && plantData->getCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        if (readyToFire) {
            mediator->addProjectile(getType(), getProjectileSpawnPosition(), getDamage());
            
            PlantType pType = getType();
            if (pType == PUFFSHROOM || pType == SEASHROOM || pType == SCAREDYSHROOM) {
                mediator->playSound("PUFF", 0.65f);
            } else if (pType == FUMESHROOM || pType == GLOOMSHROOM) {
                mediator->playSound("FUME", 0.65f);
            } else if (pType == CORNPULT) {
                mediator->playSound(GetRandomValue(0, 1) ? "KERNELPULT" : "KERNELPULT2", 0.65f);
            } else if (pType == COBCANNON) {
                mediator->playSound("COBLAUNCH", 0.8f);
            } else {
                mediator->playSound(GetRandomValue(0, 1) ? "THROW" : "THROW2", 0.6f);
            }

            if (getType() == CABBAGEPULT) {
                animation.hideTrack("Cabbagepult_cabbage");
            } else if (getType() == CORNPULT) {
                animation.hideTrack("Cornpult_kernal");
                animation.hideTrack("Cornpult_butter");
            } else if (getType() == MELONPULT || getType() == WINTERMELON) {
                animation.hideTrack("Melonpult_mellon");
            }

            readyToFire = false;
            resetAnimTimer = 0.2f * plantData -> getCooldown(); // 20% of the cooldown to reload the projectile  
            resetCooldown();
        }

        if (fireDelayTimer <= 0.0f && !isOnCooldown()) {
            if (!mediator->hasTarget(getType(), getProjectileSpawnPosition(), getBounds())) return;

            triggerActionAnimation();
            
            if (getType() == CABBAGEPULT || getType() == CORNPULT || getType() == MELONPULT || getType() == WINTERMELON) {
                fireDelayTimer = 0.20f; // estimated delay for lobbing animation
            } else {
                fireDelayTimer = 0.10f;
            }
        }
    }
}
