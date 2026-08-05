#include <Screens/LoadScreen.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
    constexpr float VIRTUAL_WIDTH = 800.0f;
    constexpr float VIRTUAL_HEIGHT = 600.0f;

    constexpr float BAR_X = 239.5f;
    constexpr float DIRT_BAR_Y = 500.0f;
    constexpr float GRASS_BAR_X = 243.0f;
    constexpr float GRASS_BAR_Y = 507.0f;

    constexpr std::array<float, 5> SPROUT_THRESHOLDS = {
        0.16f, 0.33f, 0.50f, 0.67f, 0.84f
    };

    constexpr std :: array<Vector2, 5> SPROUT_POSITIONS = {{
        {285.0f, 519.0f},
        {335.0f, 519.0f},
        {385.0f, 519.0f},
        {435.0f, 519.0f},
        {485.0f, 519.0f}
    }};

    constexpr Vector2 ZOMBIE_HEAD_POSITION = {535.0f, 519.0f};
}

LoadScreen::LoadScreen(
    int screenWidth,
    int screenHeight,
    AssetManager* manager
) : screenWidth(screenWidth), screenHeight(screenHeight) {
    setAssetManager(manager);
    if (assetManager == nullptr) {
        TraceLog(LOG_ERROR, "LoadScreen: AssetManager is null");
        return;
    }

    TextureManager* textureManager = assetManager->getTextureManager();
    AnimationManager* animationManager = assetManager->getAnimationManager();
    textManager = assetManager->getTextManager();
    if (textureManager == nullptr || animationManager == nullptr) {
        TraceLog(LOG_ERROR, "LoadScreen: asset managers are unavailable");
        return;
    }

    TexturePackage* package = textureManager->getPackage("LoadMenu");
    if (package == nullptr) {
        TraceLog(LOG_ERROR, "LoadScreen: LoadMenu texture package is unavailable");
        return;
    }

    background = package->GetTexture("TITLESCREEN");
    logo = package -> GetTexture("PVZ_LOGO");
    dirtBar = package -> GetTexture("LOADBAR_DIRT");
    grassBar = package -> GetTexture("LOADBAR_GRASS");

    ReanimParser* sproutParser = animationManager -> getAnimationData("LoadBarSproutAnim");
    if (sproutParser != nullptr) {
        sproutAnimationReady = true;
        for (ReanimInstance& sprout : sprouts) {
            sprout.setAnimation(sproutParser);
            sprout.setTexturePackage(package);
            sprout.setTextureScalar(getScale());
            sproutAnimationReady = sprout.playClip("sprout") && sproutAnimationReady;
            sprout.setLoopToggle(false);
        }
    }

    ReanimParser* zombieParser =
        animationManager->getAnimationData("LoadBarZombieheadAnim");
    if (zombieParser != nullptr) {
        zombieHead.setAnimation(zombieParser);
        zombieHead.setTexturePackage(package);
        zombieHead.setTextureScalar(getScale());
        zombieAnimationReady = zombieHead.playClip("zombie");
        zombieHead.setLoopToggle(false);
    }

    if (background == nullptr || logo == nullptr ||
        dirtBar == nullptr || grassBar == nullptr) {
        TraceLog(LOG_ERROR, "LoadScreen: one or more required textures are missing");
    }
}

float LoadScreen :: getScale() const {
    return std::min(
        static_cast<float>(screenWidth) / VIRTUAL_WIDTH,
        static_cast<float>(screenHeight) / VIRTUAL_HEIGHT
    );
}

Vector2 LoadScreen :: getOffset() const {
    const float scale = getScale();
    return {
        (static_cast<float>(screenWidth) - VIRTUAL_WIDTH * scale) * 0.5f,
        (static_cast<float>(screenHeight) - VIRTUAL_HEIGHT * scale) * 0.5f
    };
}

Rectangle LoadScreen :: toScreenBounds(Rectangle virtualBounds) const {
    const float scale = getScale();
    const Vector2 offset = getOffset();
    return {
        offset.x + virtualBounds.x * scale,
        offset.y + virtualBounds.y * scale,
        virtualBounds.width * scale,
        virtualBounds.height * scale
    };
}

void LoadScreen :: update(float dt) {
    if (assetManager == nullptr) return;

    assetManager->updateLoading(6.0);

    const float realProgress = assetManager->getLoadingProgress();
    displayedProgress = std::min(
        realProgress,
        displayedProgress + dt * 0.45f
    );

    if (sproutAnimationReady) {
        for (std::size_t index = 0; index < sprouts.size(); ++index) {
            if (!sproutActive[index] && displayedProgress >= SPROUT_THRESHOLDS[index]) {
                sproutActive[index] = true;
            }
            if (sproutActive[index]) {
                sprouts[index].updateTime(dt);
            }
        }
    }

    if (phase == Phase :: LOADING &&
        assetManager->isLoadingFinished() &&
        displayedProgress >= 0.999f) {
        displayedProgress = 1.0f;
        phase = zombieAnimationReady ? Phase::FINISHING : Phase::READY;
    }

    if (phase == Phase :: FINISHING) {
        zombieHead.updateTime(dt);
        if (zombieHead.isFinished()) {
            phase = Phase :: READY;
        }
    }

    if (phase == Phase :: READY) {
        blinkTime += dt;
    }
}

void LoadScreen :: drawLoadingBar() const {
    if (dirtBar == nullptr || grassBar == nullptr) return;

    const float scale = getScale();
    const Vector2 offset = getOffset();

    DrawTextureEx(
        *dirtBar,
        {offset.x + BAR_X * scale, offset.y + DIRT_BAR_Y * scale},
        0.0f,
        scale,
        WHITE
    );

    const float visibleWidth = grassBar->width * displayedProgress;
    if (visibleWidth > 0.0f) {
        DrawTexturePro(
            *grassBar,
            {0.0f, 0.0f, visibleWidth, static_cast<float>(grassBar->height)},
            {
                offset.x + GRASS_BAR_X * scale,
                offset.y + GRASS_BAR_Y * scale,
                visibleWidth * scale,
                grassBar->height * scale
            },
            {0.0f, 0.0f},
            0.0f,
            WHITE
        );
    }

    if (sproutAnimationReady) {
        for (std::size_t index = 0; index < sprouts.size(); ++index) {
            if (!sproutActive[index]) continue;

            sprouts[index].draw({
                offset.x + SPROUT_POSITIONS[index].x * scale,
                offset.y + SPROUT_POSITIONS[index].y * scale,
                0.0f,
                0.0f
            });
        }
    }

    if (phase != Phase::LOADING && zombieAnimationReady) {
        zombieHead.draw({
            offset.x + ZOMBIE_HEAD_POSITION.x * scale,
            offset.y + ZOMBIE_HEAD_POSITION.y * scale,
            0.0f,
            0.0f
        });
    }
}

void LoadScreen :: drawStatusText() const {
    if (textManager == nullptr) return;

    const Rectangle textBounds = toScreenBounds({200.0f, 555.0f, 400.0f, 35.0f});
    const float fontSize = 24.0f * getScale();

    if (phase == Phase::READY) {
        const float pulse = 0.5f + 0.5f * std::sin(blinkTime * 4.0f);
        const unsigned char alpha = static_cast<unsigned char>(130.0f + pulse * 125.0f);
        textManager->drawCenteredText(
            "LUCKIESTGUY-REGULAR",
            "CLICK TO START",
            textBounds,
            fontSize,
            getScale(),
            {255, 255, 255, alpha}
        );
        return;
    }

    char loadingText[32];
    std::snprintf(
        loadingText,
        sizeof(loadingText),
        "LOADING... %d%%",
        static_cast<int>(displayedProgress * 100.0f)
    );
    textManager -> drawCenteredText(
        "LUCKIESTGUY-REGULAR",
        loadingText,
        textBounds,
        fontSize,
        getScale(),
        WHITE
    );
}

void LoadScreen::draw() {
    const float scale = getScale();
    const Vector2 offset = getOffset();

    if (background != nullptr) {
        DrawTextureEx(*background, offset, 0.0f, scale, WHITE);
    }
    if (logo != nullptr) {
        DrawTextureEx(
            *logo,
            {offset.x + 50.0f * scale, offset.y + 24.0f * scale},
            0.0f,
            scale,
            WHITE
        );
    }

    drawLoadingBar();
    drawStatusText();
}

void LoadScreen::handleInput(const RawInputEvent& inputEvent) {
    if (phase != Phase::READY) return;

    const bool mouseClicked =
        inputEvent.inputType == RawInputEvent::InputType::LEFT_MOUSE_CLICKED;
    const bool confirmKey =
        inputEvent.inputType == RawInputEvent::InputType::KEY_PRESSED &&
        (inputEvent.keySignal == KEY_ENTER || inputEvent.keySignal == KEY_SPACE);

    if (mouseClicked || confirmKey) {
        requestTransition(ScreenAction::REPLACE, ScreenID::MAIN_MENU);
    }
}
