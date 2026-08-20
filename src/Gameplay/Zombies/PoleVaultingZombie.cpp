#include "Gameplay/Zombies/PoleVaultingZombie.hpp"
#include "Gameplay/Plants/Plant.hpp"
#include <algorithm>

void PoleVaultingZombie::zombieSetup() {
    Zombie::zombieSetup();
    hasVaulted = false;
    isVaulting = false;
    hasDroppedPoleParticle = false;
    mediatorRef = nullptr;
    vaultProgress = 0.0f;
    vaultDuration = 1.3f;
    vaultOffsetX = 0.0f;
    vaultOffsetY = 0.0f;
    speed = 32.0f; // Fast sprint while holding pole
}

Rectangle PoleVaultingZombie::getHitbox() const {
    return {
        hitbox.x + vaultOffsetX,
        hitbox.y + vaultOffsetY,
        hitbox.width,
        hitbox.height
    };
}

void PoleVaultingZombie::updateTime(float dt) {
    if (isDead() || isDying()) {
        isVaulting = false;
        vaultOffsetX = 0.0f;
        vaultOffsetY = 0.0f;
        Zombie::updateTime(dt);
        return;
    }

    if (isVaulting) {
        vaultProgress += dt / vaultDuration;
        animation.updateTime(dt);

        const float dir = 1.0f;

        // Phase 1 (0.0 to 0.22): Sticking pole straight down to ground in front of plant
        if (vaultProgress < 0.22f) {
            vaultOffsetX = 0.0f;
            vaultOffsetY = 0.0f;
        }
        // Phase 2 (0.22 to 0.85): Parabolic vault arc over the plant while holding the pole
        else if (vaultProgress < 0.85f) {
            float p = (vaultProgress - 0.22f) / 0.63f;
            vaultOffsetX = Lerp(0.0f, -120.0f * dir, p);
            vaultOffsetY = -sinf(p * PI) * 85.0f; // High parabolic arc
        }
        // Phase 3 (0.85 to 1.0): Landed on the ground behind the plant
        else {
            vaultOffsetX = -120.0f * dir;
            vaultOffsetY = 0.0f;
        }

        if (vaultProgress >= 1.0f || animation.isFinished()) {
            hitbox.x += -120.0f * dir;
            vaultOffsetX = 0.0f;
            vaultOffsetY = 0.0f;
            isVaulting = false;
            hasVaulted = true;
            speed = 18.0f; // Normal walking speed after dropping pole
            animation.setSpeed(1.0f);
            animation.hideTrack("Zombie_polevaulter_pole");
            animation.hideTrack("Zombie_polevaulter_pole2");
            animation.playClip("anim_walk");
            animation.setLoopToggle(true);
        }
        return;
    }

    Zombie::updateTime(dt);
}

void PoleVaultingZombie::onCustomCombat(float dt, IGameplayMediator& mediator) {
    mediatorRef = &mediator;

    if (!hasVaulted && !isVaulting && state == ZombieState::WALKING) {
        Plant* targetPlant = mediator.getPlantInArea(getAttackHitbox());
        if (targetPlant != nullptr) {
            if (targetPlant->getType() == TALLNUT) {
                // TallNut is too tall to vault over!
                // Pole vaulter smashes into the TallNut, drops the pole immediately, and starts eating.
                hasVaulted = true;
                isVaulting = false;
                hasDroppedPoleParticle = true;
                speed = 18.0f;
                animation.setSpeed(1.0f);
                animation.hideTrack("Zombie_polevaulter_pole");
                animation.hideTrack("Zombie_polevaulter_pole2");
                animation.playClip("anim_walk");
                animation.setLoopToggle(true);
                mediator.playSound("POLEVAULT", 1.0f);

                // Spawn dropped pole particle falling onto the lawn in front of TallNut
                TexturePackage* pkg = animation.getTexturePackage();
                Texture2D* poleTex = pkg ? pkg->GetTexture("ZOMBIE_POLEVAULTER_POLE") : nullptr;
                if (poleTex) {
                    auto poleP = std::make_unique<Particle>(
                        poleTex,
                        Vector2{ hitbox.x - 10.0f, hitbox.y + 35.0f },
                        Vector2{ 20.0f, -40.0f },
                        Vector2{ 0.0f, 260.0f },
                        1.5f,
                        1.25f
                    );
                    poleP->setRotation(20.0f);
                    poleP->setAngularVelocity(40.0f);
                    mediator.addParticle(std::move(poleP));
                }
            } else {
                hasVaulted = true;
                isVaulting = true;
                hasDroppedPoleParticle = true;
                vaultProgress = 0.0f;
                vaultOffsetX = 0.0f;
                vaultOffsetY = 0.0f;
                animation.setSpeed(2.7f); // Synchronize 42-frame jump to 1.3s vault duration
                animation.playClip("anim_jump");
                animation.setLoopToggle(false);
                mediator.playSound("POLEVAULT", 1.0f);

                // Spawn thrown pole particle at first to touch the ground as the vault begins!
                TexturePackage* pkg = animation.getTexturePackage();
                Texture2D* poleTex = pkg ? pkg->GetTexture("ZOMBIE_POLEVAULTER_POLE") : nullptr;
                if (poleTex) {
                    auto poleP = std::make_unique<Particle>(
                        poleTex,
                        Vector2{ hitbox.x - 25.0f, hitbox.y + 35.0f },
                        Vector2{ -35.0f, -30.0f },
                        Vector2{ 0.0f, 180.0f },
                        1.6f,
                        1.25f
                    );
                    poleP->setRotation(-35.0f);
                    poleP->setAngularVelocity(-20.0f);
                    mediator.addParticle(std::move(poleP));
                }
            }
        }
    }
}
