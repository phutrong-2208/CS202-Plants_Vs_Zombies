#include "Gameplay/Zombies/JackInTheBoxZombie.hpp"
#include <algorithm>

void JackInTheBoxZombie::zombieSetup() {
    Zombie::zombieSetup();
    speed = 26.0f; // Fast running pace winding music box
    explodeTimer = static_cast<float>(GetRandomValue(150, 240)) / 10.0f; // 15.0s - 24.0s
    isPopping = false;
    popTimer = 0.0f;
}

void JackInTheBoxZombie::updateTime(float dt) {
    if (isPopping) {
        popTimer -= dt;
        animation.updateTime(dt);
        return;
    }

    if (explodeTimer > 0.0f) {
        explodeTimer -= dt;
    }

    Zombie::updateTime(dt);
}

void JackInTheBoxZombie::onCustomCombat(float dt, IGameplayMediator& mediator) {
    if (state == ZombieState::DYING || state == ZombieState::DEAD) return;

    if (isPopping) {
        if (popTimer <= 0.0f || animation.isFinished()) {
            // Detonate like a Cherry Bomb in 3x3 area, instantly destroying all plants!
            Rectangle blastArea = {
                hitbox.x + hitbox.width * 0.5f - 130.0f,
                hitbox.y + hitbox.height * 0.5f - 130.0f,
                260.0f,
                260.0f
            };

            mediator.killPlantsInArea(blastArea);
            mediator.spawnExplosionParticles(Vector2{ hitbox.x, hitbox.y }, CHERRYBOMB);

            // Self-destruct
            setState(ZombieState::DEAD);
        }
        return;
    }

    // Pop the music box only when the song timer finishes
    if (explodeTimer <= 0.0f) {
        isPopping = true;
        popTimer = 0.45f;
        speed = 0.0f;
        animation.playClip("anim_pop");
        animation.setLoopToggle(false);
    }
}
