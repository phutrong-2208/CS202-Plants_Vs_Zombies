#include "Gameplay/Particle/Sun.hpp"

Sun :: Sun(ReanimInstance animation, Vector2 position, float targetY, int value) : 
    Particle(nullptr, position, {}, {}, 10.0f, 1.0f),
    value(value),
    targetY(targetY),
    animation(std :: move(animation)) {}

void Sun :: update(float dt) {
    if(expired) return;

    animation.updateTime(dt);

    if(state == SunState :: DROPPING){
        position.y += 90.0f * dt;
        if(position.y >= targetY){
            position.y = targetY;
            state = SunState :: IDLE;
        }
        return;
    }

    if(state == SunState :: IDLE){
        lifeTime -= dt;
        if(lifeTime <= 0.0f) expired = true;
    }
}

void Sun :: draw() const {
    if(expired) return;
    animation.draw({position.x, position.y, 0.0f, 0.0f});
}

int Sun :: collect(Vector2 mousePosition) {
    if(expired || collected || !CheckCollisionPointRec(mousePosition, getHitBox())) {
        return 0;
    }

    collected = true;
    state = SunState :: COLLECTING;
    expired = true;
    return value;
}

Rectangle Sun :: getHitBox() const {
    return {
        position.x - 45.0f, position.y - 45.0f, 90.0f, 90.0f
    };
}

