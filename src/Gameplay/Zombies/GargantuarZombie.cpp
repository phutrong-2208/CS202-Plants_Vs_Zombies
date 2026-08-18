#include "Gameplay/Zombies/GargantuarZombie.hpp"
#include <algorithm>

void GargantuarZombie::zombieSetup() {
    Zombie::zombieSetup();
    hasThrownImp = false;
    isThrowing = false;
    throwTimer = 0.0f;
    isSmashing = false;
    smashTimer = 0.0f;
    smashCooldown = 0.0f;
    speed = 12.0f; // Giant heavy walk speed
}

void GargantuarZombie::updateTime(float dt) {
    if (smashCooldown > 0.0f) {
        smashCooldown -= dt;
    }

    if (isThrowing) {
        throwTimer -= dt;
        animation.updateTime(dt);
        if (throwTimer <= 0.0f || animation.isFinished()) {
            isThrowing = false;
            animation.playClip("anim_walk");
            animation.setLoopToggle(true);
        }
        return;
    }

    if (isSmashing) {
        smashTimer -= dt;
        animation.updateTime(dt);
        if (smashTimer <= 0.0f || animation.isFinished()) {
            isSmashing = false;
            animation.playClip("anim_walk");
            animation.setLoopToggle(true);
        }
        return;
    }

    Zombie::updateTime(dt);
}

void GargantuarZombie::onCustomCombat(float dt, IGameplayMediator& mediator) {
    if (isThrowing || isSmashing || state != ZombieState::WALKING) return;

    // Check if a plant is right in front to smash
    if (smashCooldown <= 0.0f && mediator.hasPlantInArea(getAttackHitbox())) {
        isSmashing = true;
        smashTimer = 0.7f;
        smashCooldown = 1.5f;
        animation.playClip("anim_smash");
        animation.setLoopToggle(false);
        // Instant lethal smash damage (telephone pole crush)
        mediator.killPlantsInArea(getAttackHitbox());
    }
}

void GargantuarZombie::receiveDamage(float damage, IGameplayMediator* mediator) {
    Zombie::receiveDamage(damage, mediator);

    // Throw baby zombie (Imp) when HP drops below 50% (1500 HP)
    if (!hasThrownImp && health > 0.0f && health <= 1500.0f && mediator) {
        hasThrownImp = true;
        isThrowing = true;
        throwTimer = 0.6f;
        animation.playClip("anim_throw");
        animation.setLoopToggle(false);

        // Hide Imp tracks from Gargantuar's back
        animation.hideTrack("Zombie_imp_innerarm_upper");
        animation.hideTrack("Zombie_imp_innerleg_foot");
        animation.hideTrack("Zombie_imp_innerleg_lower");
        animation.hideTrack("Zombie_imp_innerleg_upper");
        animation.hideTrack("Zombie_imp_outerleg_foot");
        animation.hideTrack("Zombie_imp_outerleg_lower");
        animation.hideTrack("Zombie_imp_outerleg_upper");
        animation.hideTrack("Zombie_imp_body2");
        animation.hideTrack("Zombie_imp_body1");
        animation.hideTrack("Zombie_imp_head");
        animation.hideTrack("Zombie_imp_jaw");
        animation.hideTrack("Zombie_imp_innerarm_lower");
        animation.hideTrack("Zombie_imp_outerarm_upper");
        animation.hideTrack("Zombie_imp_outerarm_lower");
        animation.hideTrack("Zombie_gargantuar_whiterope");
        animation.hideTrack("Zombie_gargantuar_rope");

        // Launch baby zombie (Imp) forward into column 2–3 in same row
        float targetImpX = std::max(180.0f, hitbox.x - 260.0f);
        mediator->spawnZombieAt(IMP_ZOMBIE, Vector2{ targetImpX, hitbox.y });
    }
}
