#include <Worlds/DayMap.hpp>

DayMap :: DayMap(
    AssetManager* assetManager,
    int grassLanes,
    bool useSodRollIntro
) : grassLaneCount(grassLanes) {
    const bool usesPartialLawn = grassLaneCount == 1 || grassLaneCount == 3;
    const std :: string backgroundName =
        usesPartialLawn ? "background1unsodded.jpg" : "background1.jpg";
    const std::string backgroundDir =
        std::string(PROJECT_DIR) + "assets/texture/Maps/Day/" + backgroundName;

    loadBackground(backgroundDir, {210, 0, 800, 600}, {250, 85, 750, 495});

    if(!usesPartialLawn || assetManager == nullptr) return;

    TexturePackage* dayPackage =
        assetManager -> getTextureManager() -> getPackage("Day");
    if(dayPackage == nullptr) {
        TraceLog(LOG_WARNING, "DayMap: texture package 'Day' was not found");
        return;
    }

    sodTexture = dayPackage -> GetTexture(
        grassLaneCount == 1 ? "SOD1ROW" : "SOD3ROW"
    );

    sodRollData = assetManager -> getAnimationManager() ->
        getAnimationData("SodRollAnim");

    sodRollEnabled = useSodRollIntro && sodTexture != nullptr && sodRollData != nullptr;
    sodRollFinished = !sodRollEnabled;
    sodProgress = sodRollEnabled ? 0.0f : 1.0f;

    if(sodRollEnabled) {
        sodRollAnimation.setTexturePackage(dayPackage);
        sodRollAnimation.setAnimation(sodRollData);
        sodRollAnimation.setTextureScalar(getViewportDest().width / crop.width);
        sodRollAnimation.setLoopToggle(false);
    }
}

void DayMap :: update(float dt) {
    Map :: update(dt);

    if(!sodRollEnabled || sodRollFinished || !Map :: isReady()) return;

    sodRollAnimation.updateTime(dt);

    const float duration = sodRollData -> getDuration();
    if(duration <= 0.0f) {
        sodProgress = 1.0f;
        sodRollFinished = true;
        return;
    }

    sodProgress = std :: min(1.0f, sodRollAnimation.getCurrentTime() / duration);
    if(sodRollAnimation.isFinished()) {
        sodProgress = 1.0f;
        sodRollFinished = true;
    }
}

void DayMap :: drawBackground() const {
    Map :: drawBackground();

    // The partial lawn belongs to the normal lawn view, not the right-side preview.
    if(state != State :: READY || sodTexture == nullptr) return;

    drawSod();

    if(sodRollEnabled && !sodRollFinished) {
        const Rectangle viewport = getViewportDest();
        sodRollAnimation.draw(viewport);
    }
}

bool DayMap :: isReady() const {
    return Map :: isReady() && sodRollFinished;
}

Vector2 DayMap :: getSodVirtualPosition() const {
    // Positions are measured against background1.jpg after its x=210 viewport crop.
    return grassLaneCount == 1
        ? Vector2{29.0f, 265.0f}
        : Vector2{25.0f, 149.0f};
}

void DayMap :: drawSod() const {
    if(sodTexture == nullptr || sodProgress <= 0.0f) return;

    const Rectangle viewport = getViewportDest();
    const float scale = viewport.width / crop.width;
    const float visibleWidth = sodTexture -> width * sodProgress;
    const Vector2 virtualPosition = getSodVirtualPosition();

    const Rectangle source = {
        0.0f,
        0.0f,
        visibleWidth,
        static_cast<float>(sodTexture -> height)
    };
    const Rectangle destination = {
        viewport.x + virtualPosition.x * scale,
        viewport.y + virtualPosition.y * scale,
        visibleWidth * scale,
        sodTexture -> height * scale
    };

    DrawTexturePro(
        *sodTexture,
        source,
        destination,
        {0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

// Rectangle DayMap :: getGridArea() const {
//     int sw = GetScreenWidth();
//     int sh = GetScreenHeight();
//     float scaleX = ((float)sw / crop.width);
//     float scaleY = ((float)sh / crop.height);

//     float dstX = (sw - crop.width * scaleX) / 2.0f;
//     float dstY = (sh - crop.height * scaleY) / 2.0f;

//     return {dstX + (lawnSrc.x - crop.x) * scaleX,
//             dstY + (lawnSrc.y - crop.y) * scaleY,
//             lawnSrc.width * scaleX, lawnSrc.height * scaleY};
// }
