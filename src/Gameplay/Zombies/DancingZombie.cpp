#include "Gameplay/Zombies/DancingZombie.hpp"
#include <algorithm>

void DancingZombie::zombieSetup() {
    Zombie::zombieSetup();
    speed = 20.0f;
    summonCooldown = 5.0f;
    isSummoning = false;
    summonTimer = 0.0f;
}

void DancingZombie::updateTime(float dt) {
    if (isSummoning) {
        summonTimer -= dt;
        animation.updateTime(dt);
        if (summonTimer <= 0.0f || animation.isFinished()) {
            isSummoning = false;
            animation.playClip("walk");
            animation.setLoopToggle(true);
        }
        return;
    }

    if (summonCooldown > 0.0f) {
        summonCooldown -= dt;
    }

    Zombie::updateTime(dt);
}

void DancingZombie::onCustomCombat(float dt, IGameplayMediator& mediator) {
    if (state != ZombieState::WALKING || isSummoning) return;

    if (summonCooldown <= 0.0f && hitbox.x < 850.0f) {
        isSummoning = true;
        summonTimer = 0.8f;
        summonCooldown = 15.0f;
        animation.playClip("anim_armraise");
        animation.setLoopToggle(false);

        // Summon 4 backup dancers (North, South, East, West)
        // North (same column, higher row)
        if (hitbox.y > 40.0f) {
            mediator.spawnZombieAt(BACKUP_DANCER_ZOMBIE, Vector2{ hitbox.x, hitbox.y - 100.0f });
        }
        // South (same column, lower row)
        if (hitbox.y < 340.0f) {
            mediator.spawnZombieAt(BACKUP_DANCER_ZOMBIE, Vector2{ hitbox.x, hitbox.y + 100.0f });
        }
        // East (behind dancer)
        mediator.spawnZombieAt(BACKUP_DANCER_ZOMBIE, Vector2{ hitbox.x + 80.0f, hitbox.y });
        // West (ahead of dancer)
        mediator.spawnZombieAt(BACKUP_DANCER_ZOMBIE, Vector2{ std::max(100.0f, hitbox.x - 80.0f), hitbox.y });
    }
}
