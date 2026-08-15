#include <Gameplay/LawnMower.hpp>

void LawnMower::init(int laneIndex, Vector2 startPos, TexturePackage* texPack, ReanimParser* animData) {
    lane = laneIndex;
    startPosition = startPos;
    position = startPos;
    state = LawnMowerState::READY;

    if (texPack && animData) {
        reanim.setTextureScalar(scalar);
        reanim.setTexturePackage(texPack);
        reanim.setAnimation(animData);
        reanim.hideTrack("anim_tricked");
        reanim.hideTrack("LawnMower_dice");
        reanim.hideTrack("LawnMower_body_tricked");
        reanim.hideTrack("LawnMower_engine_tricked");
        reanim.hideTrack("LawnMower_exhaust_tricked");
        reanim.setDefaultClip("anim_normal");
    }
}

void LawnMower::reset() {
    position = startPosition;
    state = LawnMowerState::READY;
    reanim.playClip("anim_normal");
}

void LawnMower::trigger() {
    if (state == LawnMowerState::READY) {
        state = LawnMowerState::ROLLING;
    }
}

void LawnMower::update(float dt, IGameplayMediator* mediator) {
    if (state == LawnMowerState::READY) {
        reanim.updateTime(dt * 0.4f);
        if (mediator) {
            Rectangle trigBox = getTriggerHitbox();
            if (mediator->hasZombieInArea(trigBox)) {
                trigger();
            }
        }
    } else if (state == LawnMowerState::ROLLING) {
        position.x += speed * dt;
        reanim.updateTime(dt * 3.0f);

        if (mediator) {
            Rectangle mowerHit = getHitbox();
            mediator->damageZombiesInArea(mowerHit, 10000.0f, nullptr);
        }

        if (position.x > GetScreenWidth() + 60.0f) {
            state = LawnMowerState::GONE;
        }
    }
}

void LawnMower::draw() const {
    if (state == LawnMowerState::GONE) return;
    reanim.draw(getHitbox());
}

Rectangle LawnMower::getHitbox() const {
    return Rectangle{
        position.x,
        position.y,
        65.0f,
        55.0f
    };
}

Rectangle LawnMower::getTriggerHitbox() const {
    return Rectangle{
        position.x + 10.0f,
        position.y - 15.0f,
        60.0f,
        80.0f
    };
}
