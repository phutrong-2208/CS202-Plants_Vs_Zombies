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
    slideTimer += dt;
    if (slideTimer >= slideDuration) {
        slideTimer = slideDuration;
        if (slidePhase == SlidePhase :: PAN_RIGHT) {
            slidePhase = SlidePhase :: PAN_LEFT;
            slideTimer = 0.0f;
        } else {
            state = State :: READY;
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
    }

    DrawTexturePro(background, src, getViewportDest(), {0, 0}, 0, WHITE);
}

bool Map :: isReady() const {
    return state == State :: READY;
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
