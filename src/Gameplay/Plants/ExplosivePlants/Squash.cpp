#include "Gameplay/Plants/ExplosivePlants/Squash.hpp"
#include "Gameplay/Zombies/Zombie.hpp"
#include <cmath>

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
            jumpOffsetY = 0.0f;
        }
    } else if (squashState == SquashState::JUMPING_UP) {
        animation.updateTime(deltaSeconds);
        // Smooth vertical jump arc upward
        jumpOffsetY = std::max(-75.0f, jumpOffsetY - deltaSeconds * 190.0f);

        if (animation.isFinished()) {
            squashState = SquashState::JUMPING_DOWN;
            animation.playClip("anim_jumpdown");
            animation.setLoopToggle(false);
        }
    } else if (squashState == SquashState::JUMPING_DOWN) {
        animation.updateTime(deltaSeconds);
        // Rapid fall down to slam into the ground
        jumpOffsetY = std::min(0.0f, jumpOffsetY + deltaSeconds * 300.0f);

        if (animation.isFinished() || jumpOffsetY >= 0.0f) {
            jumpOffsetY = 0.0f;
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
                
                // isExplosion = false -> Normal zombie decapitation death particles!
                lastMediator->damageZombiesInArea(crushArea, plantData -> getDamage(false), nullptr, false);
                damageDealt = true;
            }
        }
    } else if (squashState == SquashState::SQUASHED) {
        // Hold the squashed flattened frame on the ground without looping back to idle
        stateTimer -= deltaSeconds;
        jumpOffsetY = 0.0f;
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
            stateTimer = 0.25f;
            animationStarted = true;
        }
    }
}

void Squash::draw(Rectangle hitbox) {
    Rectangle renderHitbox = {
        hitbox.x,
        hitbox.y + jumpOffsetY,
        hitbox.width,
        hitbox.height
    };
    animation.draw(renderHitbox);
}
