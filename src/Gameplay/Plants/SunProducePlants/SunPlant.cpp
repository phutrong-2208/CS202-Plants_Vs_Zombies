#include "Gameplay/Plants/SunProducePlants/SunPlant.hpp"

void SunPlant::triggerBlush(float duration) {
    blushTimer = duration;
}

void SunPlant::updateTime(float deltaSeconds) {
    Plant::updateTime(deltaSeconds);
    if(blushTimer > 0.0f){
        blushTimer -= deltaSeconds;
    }
}

void SunPlant::draw(Rectangle hitbox) {
    Color tint = WHITE;
    if (blushTimer > 0.0f) {
        tint = {255, 180, 180, 255}; // Pinkish blush
    }
    animation.draw(hitbox, tint);
}
