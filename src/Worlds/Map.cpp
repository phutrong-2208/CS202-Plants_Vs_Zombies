#include "Worlds/Map.hpp"

void Map :: update(float dt) {
    if (state != State :: SLIDING) return;
    slideTimer += dt;
    if (slideTimer >= slideDuration) {
        slideTimer = slideDuration;
        state = State :: READY;
    }
}

bool Map :: isReady() const {
    return state == State :: READY;
}

Rectangle Map :: getGridArea() const {
    return {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
}

float Map :: getSlideProgress() const {
    float t = slideTimer / slideDuration;
    if (t > 1.0f) t = 1.0f;
    return 1.0f - (1.0f - t) * (1.0f - t);
}
