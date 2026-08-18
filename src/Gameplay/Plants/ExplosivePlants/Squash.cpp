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
            jumpProgress = 0.0f;
            jumpOffsetX = 0.0f;
            jumpOffsetY = 0.0f;
        }
    } else if (squashState == SquashState::JUMPING_UP) {
        animation.updateTime(deltaSeconds);
        jumpProgress += deltaSeconds / jumpUpDuration;
        float t = std::min(1.0f, jumpProgress);

        // Smooth parabolic rise up to peak height
        jumpOffsetY = -90.0f * std::sin(t * 1.5707963f);
        // Smoothly cover 65% of horizontal distance to target
        jumpOffsetX = (targetX - startX) * (0.65f * t);

        if (t >= 1.0f || animation.isFinished()) {
            squashState = SquashState::JUMPING_DOWN;
            animation.playClip("anim_jumpdown");
            animation.setLoopToggle(false);
            jumpProgress = 0.0f;
        }
    } else if (squashState == SquashState::JUMPING_DOWN) {
        animation.updateTime(deltaSeconds);
        jumpProgress += deltaSeconds / jumpDownDuration;
        float t = std::min(1.0f, jumpProgress);

        // Accelerate downward rapidly towards the ground
        jumpOffsetY = -90.0f * (1.0f - t * t);
        // Finish remaining 35% of horizontal distance
        jumpOffsetX = (targetX - startX) * (0.65f + 0.35f * t);

        if (t >= 1.0f || animation.isFinished() || jumpOffsetY >= 0.0f) {
            jumpOffsetY = 0.0f;
            jumpOffsetX = targetX - startX;
            squashState = SquashState::SQUASHED;
            stateTimer = squashedLingerDuration;

            // Apply lethal crush damage immediately upon landing!
            if (lastMediator && !damageDealt) {
                float landingX = bounds.x + jumpOffsetX;
                Rectangle crushArea = {
                    landingX - bounds.width * 0.75f,
                    bounds.y - 40.0f,
                    bounds.width * 2.5f,
                    bounds.height + 80.0f
                };
                
                // isExplosion = false -> Normal zombie decapitation death particles
                lastMediator->damageZombiesInArea(crushArea, plantData ? plantData->getDamage(false) : 1800.0f, false);
                lastMediator->playSound("GULP", 1.0f);
                damageDealt = true;
            }
        }
    } else if (squashState == SquashState::SQUASHED) {
        // Hold the squashed flattened frame on the ground
        stateTimer -= deltaSeconds;
        jumpOffsetY = 0.0f;
        if (stateTimer <= 0.0f) {
            squashState = SquashState::DONE;
            health = 0; // Dies and gets removed from the cell
        }
    }
}

void Squash::performAction(IGameplayMediator* mediator) {
    if (!mediator || health <= 0) return;
    lastMediator = mediator;

    if (squashState == SquashState::IDLE) {
        // Detect zombies up to 1.5 tiles ahead or behind in same row
        Rectangle detectArea = {
            bounds.x - bounds.width * 1.5f,
            bounds.y - 40.0f,
            bounds.width * 4.0f,
            bounds.height + 80.0f
        };

        if (mediator->hasZombieInArea(detectArea)) {
            mediator->playSound("SQUASH_HMM", 1.0f);
            Zombie* target = mediator->getZombiePriority(detectArea);
            startX = bounds.x;
            if (target) {
                targetX = target->getHitbox().x + target->getHitbox().width * 0.5f - bounds.width * 0.5f;
                if (targetX < bounds.x) {
                    animation.playClip("anim_lookleft");
                } else {
                    animation.playClip("anim_lookright");
                }
            } else {
                targetX = bounds.x + 60.0f;
                animation.playClip("anim_lookright");
            }
            animation.setLoopToggle(false);
            squashState = SquashState::LOOKING;
            stateTimer = 0.25f;
        }
    }
}

void Squash::draw(Rectangle hitbox) {
    Rectangle renderHitbox = {
        hitbox.x + jumpOffsetX,
        hitbox.y + jumpOffsetY,
        hitbox.width,
        hitbox.height
    };
    animation.draw(renderHitbox);
}
