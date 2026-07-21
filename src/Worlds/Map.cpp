#include "Worlds/Map.hpp"

Map :: ~Map() {
    if (hasBackground) {
        UnloadTexture(background);
    }
}

void Map :: loadBackground(const std :: string& path, Rectangle cropRect, Rectangle lawnRect) {
    if (hasBackground) {
        UnloadTexture(background);
    }

    crop = cropRect;
    lawnSrc = lawnRect;
    background = LoadTexture(path.c_str());
    hasBackground = true;
}

void Map :: update(float dt) {
    if (state != State :: SLIDING) return;
    float safeDt = std :: min(dt, 0.05f); //prevent loading parallel with assets loading, 
    //without safeDt, right sliding phase will be skipped
    slideTimer += safeDt;
    if (slideTimer >= slideDuration) {
        if (slidePhase == SlidePhase :: PAN_RIGHT) {
            state = State :: CHOOSE_PLANTS;
            slidePhase = SlidePhase :: PAN_LEFT;
            slideTimer = slideDuration;
        } else {
            state = State :: READY;
            slideTimer = slideDuration;
        }
    }
}

void Map :: draw() const {
    drawBackground();
}

void Map :: drawBackground() const {
    if (!hasBackground) return;

    Rectangle src = crop;
    if (state == State :: SLIDING) {
        src.x = getSlidingCropX();
    } else if (state == State :: CHOOSE_PLANTS) {
        src.x = (float)background.width - crop.width;
    }

    DrawTexturePro(background, src, getViewportDest(), {0, 0}, 0, WHITE);
}

bool Map :: isReady() const {
    return state == State :: READY;
}

Map::State Map :: getState() const {
    return state;
}

bool Map :: isChoosingPlants() const {
    return state == State :: CHOOSE_PLANTS;
}

void Map :: finishChoosingPlants() {
    if (state == State :: CHOOSE_PLANTS) {
        state = State :: SLIDING;
        slidePhase = SlidePhase :: PAN_LEFT;
        slideTimer = 0.0f;
    }
}

// Rectangle Map :: getGridArea() const {
//     return {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
// }

float Map :: getSlideProgress() const {
    float t = slideTimer / slideDuration;
    if (t > 1.0f) t = 1.0f;
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float Map :: getSlidingCropX() const {
    float maxCropX = (float) background.width - crop.width;
    float eased = getSlideProgress();

    if (slidePhase == SlidePhase :: PAN_RIGHT) {
        return eased * maxCropX;
    }

    return (1.0f - eased) * maxCropX + eased * crop.x;
}

Rectangle Map :: getViewportDest() const {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float scale = std :: min(GetScreenWidth() / crop.width, GetScreenHeight() / crop.height);
    float dstW = crop.width * scale;
    float dstH = crop.height * scale;
    float dstX = (sw - dstW) / 2.0f;
    float dstY = (sh - dstH) / 2.0f;

    return {dstX, dstY, dstW, dstH};
}
