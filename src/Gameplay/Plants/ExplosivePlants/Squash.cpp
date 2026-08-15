#include "Gameplay/Plants/ExplosivePlants/Squash.hpp"
#include "Gameplay/Zombies/Zombie.hpp"

PlantType Squash::getType() { return SQUASH; }

void Squash::updateTime(float deltaSeconds) {
    if (cooldownTimer > 0.0f) {
        cooldownTimer -= deltaSeconds;
    }

    if (squashState == SquashState::IDLE) {
        animation.updateTime(deltaSeconds);
        return;
    }

    if (squashState == SquashState::LOOKING) {
        animation.updateTime(deltaSeconds);
        stateTimer -= deltaSeconds;
        if (stateTimer <= 0.0f || animation.isFinished()) {
            squashState = SquashState::JUMPING_UP;
            animation.playClip("anim_jumpup");
            animation.setLoopToggle(false);
        }
    } else if (squashState == SquashState::JUMPING_UP) {
        animation.updateTime(deltaSeconds);
        if (animation.isFinished()) {
            squashState = SquashState::JUMPING_DOWN;
            animation.playClip("anim_jumpdown");
            animation.setLoopToggle(false);
        }
    } else if (squashState == SquashState::JUMPING_DOWN) {
        animation.updateTime(deltaSeconds);
        if (animation.isFinished()) {
            squashState = SquashState::SQUASHED;
            stateTimer = squashedLingerDuration;

            // Apply lethal crush damage immediately upon landing!
            if (lastMediator && !damageDealt) {
                float minX = std::min(bounds.x - bounds.width * 1.5f, targetX - 80.0f);
                float maxX = std::max(bounds.x + bounds.width * 2.5f, targetX + 120.0f);
                Rectangle crushArea = {
                    minX,
                    bounds.y - 50.0f,
                    maxX - minX,
                    bounds.height + 100.0f
                };
                lastMediator->damageZombiesInArea(crushArea, 1800.0f);
                lastMediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, SQUASH);
                damageDealt = true;
            }
        }
    } else if (squashState == SquashState::SQUASHED) {
        // Hold the squashed pose on the ground without advancing animation or resetting to idle
        stateTimer -= deltaSeconds;
        if (stateTimer <= 0.0f) {
            squashState = SquashState::DONE;
            health = 0; // Dies and gets removed from the cell after linger delay
        }
    }
}

void Squash::performAction(IGameplayMediator* mediator) {
    if (!mediator || health <= 0) return;
    lastMediator = mediator;

    if (squashState == SquashState::IDLE) {
        Rectangle detectArea = {
            bounds.x - bounds.width * 1.5f,
            bounds.y - 40.0f,
            bounds.width * 4.0f,
            bounds.height + 80.0f
        };

        if (mediator->hasZombieInArea(detectArea)) {
            Zombie* target = mediator->getZombiePriority(detectArea);
            if (target) {
                targetX = target->getHitbox().x;
                if (targetX + target->getHitbox().width * 0.5f < bounds.x + bounds.width * 0.5f) {
                    animation.playClip("anim_lookleft");
                } else {
                    animation.playClip("anim_lookright");
                }
            } else {
                targetX = bounds.x;
                animation.playClip("anim_lookright");
            }
            animation.setLoopToggle(false);
            squashState = SquashState::LOOKING;
            stateTimer = 0.15f;
            animationStarted = true;
        }
    } else if ((squashState == SquashState::SQUASHED || (squashState == SquashState::JUMPING_DOWN && animation.isFinished())) && !damageDealt) {
        float minX = std::min(bounds.x - bounds.width * 1.5f, targetX - 80.0f);
        float maxX = std::max(bounds.x + bounds.width * 2.5f, targetX + 120.0f);
        Rectangle crushArea = {
            minX,
            bounds.y - 50.0f,
            maxX - minX,
            bounds.height + 100.0f
        };
        mediator->damageZombiesInArea(crushArea, 1800.0f);
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, SQUASH);
        damageDealt = true;
        squashState = SquashState::SQUASHED;
        stateTimer = squashedLingerDuration;
    }
}
