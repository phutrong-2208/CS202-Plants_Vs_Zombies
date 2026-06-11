#include "Worlds/DayMap.hpp"
#include "Core/TextureManager.hpp"

DayMap :: DayMap() : phase(Phase :: PAN_RIGHT), phaseTimer(0.0f), phaseDuration(2.0f){
    crop = {210, 0, 800, 600};
    lawnSrc = {250, 85, 750, 495};
}

void DayMap :: update(float dt) {
    if (phase == Phase :: DONE) return;
    phaseTimer += dt;
    if (phaseTimer >= phaseDuration) {
        phaseTimer = phaseDuration;
        if (phase == Phase :: PAN_RIGHT) {
            phase = Phase :: PAN_LEFT;
            phaseTimer = 0.0f;
        } else {
            phase = Phase :: DONE;
            state = State :: READY;
        }
    }
}

Rectangle DayMap :: getGridArea() const {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float scaleX = ((float)sw / crop.width);
    float scaleY = ((float)sh / crop.height);

    float dstX = (sw - crop.width * scaleX) / 2.0f;
    float dstY = (sh - crop.height * scaleY) / 2.0f;

    return {dstX + (lawnSrc.x - crop.x) * scaleX,
            dstY + (lawnSrc.y - crop.y) * scaleY,
            lawnSrc.width * scaleX, lawnSrc.height * scaleY};
}

void DayMap :: draw() const {
    Texture2D bg = file.texture["background1"];
    if (bg.id == 0) return;

    float maxCropX = (float)bg.width - crop.width;

    float t = phaseTimer / phaseDuration;
    if (t > 1.0f) t = 1.0f;
    float eased = 1.0f - (1.0f - t) * (1.0f - t);

    Rectangle src = crop;
    if (phase == Phase :: PAN_RIGHT) {
        src.x = eased * maxCropX;
    } else if (phase == Phase :: PAN_LEFT) {
        src.x = (1.0f - eased) * maxCropX + eased * crop.x;
    }

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float scale = ((float)sw / crop.width < (float)sh / crop.height)
                  ? (float)sw / crop.width : (float)sh / crop.height;
    float dstW = crop.width * scale;
    float dstH = crop.height * scale;
    float dstX = (sw - dstW) / 2.0f;
    float dstY = (sh - dstH) / 2.0f;

    DrawTexturePro(bg, src, {dstX, dstY, dstW, dstH}, {0, 0}, 0, WHITE);
}
