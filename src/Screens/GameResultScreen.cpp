#include <Screens/GameResultScreen.hpp>

namespace {
    constexpr Rectangle TITLE_BOUNDS = {145.0f, 24.0f, 510.0f, 48.0f};
    constexpr Rectangle REWARD_PACKET_BOUNDS = {350.0f, 133.0f, 100.0f, 140.0f};
    constexpr Rectangle REWARD_DESCRIPTION_BOUNDS = {270.0f, 354.0f, 260.0f, 100.0f};
    constexpr Rectangle CONTINUE_BUTTON_BOUNDS = {290.0f, 525.0f, 220.0f, 46.0f};

    constexpr float LOSE_ANIMATION_HOLD_TIME = 0.78f;
}

GameResultScreen :: GameResultScreen(int width, int height, AssetManager* manager, ScreenData screenData) : screenWidth(width), screenHeight(height), data(std :: move(screenData)) {
    setAssetManager(manager);
    if(!assetManager) {
        TraceLog(LOG_ERROR, "GameResultScreen: AssetManager is null");
        return;
    }

    TextureManager* textureManager = assetManager -> getTextureManager();
    AnimationManager* animationManager = assetManager -> getAnimationManager();
    textManager = assetManager -> getTextManager();

    if(!textureManager || !animationManager) {
        TraceLog(LOG_ERROR, "GameResultScreen: asset managers are unavailable");
        return;
    }

    resultPackage = textureManager -> getPackage("GameResult");
    seedPacketPackage = textureManager -> getPackage("PlantSeedPackets");

    if(resultPackage) {
        awardBackground = resultPackage -> GetTexture("AWARDSCREEN_BACK");
        buttonLeft = resultPackage -> GetTexture("BUTTON_LEFT");
        buttonMiddle = resultPackage -> GetTexture("BUTTON_MIDDLE");
        buttonRight = resultPackage -> GetTexture("BUTTON_RIGHT");
        buttonDownLeft = resultPackage -> GetTexture("BUTTON_DOWN_LEFT");
        buttonDownMiddle = resultPackage -> GetTexture("BUTTON_DOWN_MIDDLE");
        buttonDownRight = resultPackage -> GetTexture("BUTTON_DOWN_RIGHT");
    }

    ReanimParser* loseParser = animationManager -> getAnimationData("ZombiesWonAnim");
    if(resultPackage && loseParser) {
        loseAnimation.setTexturePackage(resultPackage);
        loseAnimation.setAnimation(loseParser);
        loseAnimation.setTextureScalar(getScale());
        loseAnimation.setLoopToggle(false);
    }
}

float GameResultScreen :: getScale() const {
    return std :: min(
        static_cast<float>(screenWidth) / VIRTUAL_WIDTH,
        static_cast<float>(screenHeight) / VIRTUAL_HEIGHT
    );
}

Vector2 GameResultScreen :: getOffset() const {
    const float scale = getScale();
    return {
        (static_cast<float>(screenWidth) - VIRTUAL_WIDTH * scale) * 0.5f,
        (static_cast<float>(screenHeight) - VIRTUAL_HEIGHT * scale) * 0.5f
    };
}

Rectangle GameResultScreen :: toScreenBounds(Rectangle virtualBounds) const {
    const float scale = getScale();
    const Vector2 offset = getOffset();
    return {
        offset.x + virtualBounds.x * scale,
        offset.y + virtualBounds.y * scale,
        virtualBounds.width * scale,
        virtualBounds.height * scale
    };
}

Rectangle GameResultScreen :: getContinueButtonBounds() const {
    return toScreenBounds(CONTINUE_BUTTON_BOUNDS);
}

bool GameResultScreen :: isContinueButtonHovered() const {
    return CheckCollisionPointRec(GetMousePosition(), getContinueButtonBounds());
}

void GameResultScreen :: update(float dt) {
    if(data.wResult != WorldResult :: LOST) return;

    if(loseAnimationTime < LOSE_ANIMATION_HOLD_TIME) {
        const float remaining = LOSE_ANIMATION_HOLD_TIME - loseAnimationTime;
        const float animationStep = std :: min(dt, remaining);
        loseAnimation.updateTime(animationStep);
        loseAnimationTime += animationStep;
    }
}

void GameResultScreen :: draw() {
    if(data.wResult == WorldResult :: LOST) {
        drawLoseScreen();
        return;
    }

    drawWinScreen();
}

void GameResultScreen :: drawLoseScreen() const {
    DrawRectangle(
        0, 0, screenWidth, screenHeight, Color{0, 0, 0, 185}
    );

    const Vector2 offset = getOffset();
    loseAnimation.draw({
        offset.x, offset.y, VIRTUAL_WIDTH * getScale(), VIRTUAL_HEIGHT * getScale()
    });

    if(loseAnimationTime >= LOSE_ANIMATION_HOLD_TIME) {
        drawResultButton(
            getContinueButtonBounds(),
            "MAIN MENU",
            isContinueButtonHovered()
        );
    }
}

void GameResultScreen :: drawWinScreen() const {
    drawAwardScreen();
}

void GameResultScreen :: drawAwardScreen() const {
    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

    const Rectangle screenBounds = toScreenBounds({
        0.0f, 0.0f, VIRTUAL_WIDTH, VIRTUAL_HEIGHT
    });

    if(awardBackground) {
        DrawTexturePro(
            *awardBackground, {0.0f, 0.0f,
                static_cast<float>(awardBackground -> width),
                static_cast<float>(awardBackground -> height)
            },
            screenBounds, {0.0f, 0.0f}, 0.0f, WHITE
        );
    }

    const bool hasPlantReward = data.rewardPlant != PLANT_COUNT;
    const char* title = hasPlantReward ? "YOU GOT A NEW PLANT!" : "LEVEL COMPLETE!";

    if(textManager) {
        const Rectangle titleBounds = toScreenBounds(TITLE_BOUNDS);
        const float titleSize = 30.0f * getScale();

        Rectangle shadowBounds = titleBounds;
        shadowBounds.x += 2.0f * getScale();
        shadowBounds.y += 2.0f * getScale();
        textManager -> drawCenteredText(
            "Luckiest_Guy", title, shadowBounds, titleSize, 1.0f * getScale(), Color{48, 24, 10, 220}
        );
        textManager -> drawCenteredText(
            "Luckiest_Guy", title, titleBounds, titleSize, 1.0f * getScale(), Color{255, 232, 126, 255}
        );
    }

    if(hasPlantReward && seedPacketPackage) {
        Texture2D* seedPacket = seedPacketPackage -> GetTexture(
            getPlantTextureKey(data.rewardPlant)
        );

        if(seedPacket) {
            DrawTexturePro(
                *seedPacket,
                {0.0f, 0.0f,
                    static_cast<float>(seedPacket -> width),
                    static_cast<float>(seedPacket -> height)
                },
                toScreenBounds(REWARD_PACKET_BOUNDS),
                {0.0f, 0.0f}, 0.0f, WHITE
            );
        }
    }

    if(textManager) {
        const Rectangle descriptionBounds = toScreenBounds(REWARD_DESCRIPTION_BOUNDS);
        const float scale = getScale();

        if(hasPlantReward) {
            Rectangle plantNameBounds = descriptionBounds;
            plantNameBounds.height *= 0.46f;
            textManager -> drawCenteredText(
                "Luckiest_Guy",
                getPlantTextureKey(data.rewardPlant),
                plantNameBounds,
                25.0f * scale,
                1.0f * scale,
                Color{80, 44, 18, 255}
            );

            Rectangle messageBounds = descriptionBounds;
            messageBounds.y += descriptionBounds.height * 0.45f;
            messageBounds.height *= 0.45f;
            textManager -> drawCenteredText(
                "Luckiest_Guy",
                "A NEW PLANT FOR YOUR COLLECTION!",
                messageBounds,
                14.0f * scale,
                0.5f * scale,
                Color{105, 63, 31, 255}
            );
        }
        else {
            textManager -> drawCenteredText(
                "Luckiest_Guy",
                "THE LAWN IS SAFE... FOR NOW!",
                descriptionBounds,
                18.0f * scale,
                0.5f * scale,
                Color{80, 44, 18, 255}
            );
        }
    }

    drawResultButton(
        getContinueButtonBounds(),
        "CONTINUE",
        isContinueButtonHovered()
    );
}

void GameResultScreen :: drawResultButton(Rectangle bounds, const char* label, bool hovered) const {
    Texture2D* left = hovered && buttonDownLeft ? buttonDownLeft : buttonLeft;
    Texture2D* middle = hovered && buttonDownMiddle ? buttonDownMiddle : buttonMiddle;
    Texture2D* right = hovered && buttonDownRight ? buttonDownRight : buttonRight;

    if(left && middle && right) {
        const float scale = getScale();
        const float leftWidth = left -> width * scale;
        const float rightWidth = right -> width * scale;
        const float middleWidth = std :: max(
            0.0f,
            bounds.width - leftWidth - rightWidth
        );

        DrawTexturePro(
            *left,
            {0.0f, 0.0f, static_cast<float>(left -> width), static_cast<float>(left -> height)},
            {bounds.x, bounds.y, leftWidth, bounds.height},
            {0.0f, 0.0f}, 0.0f, WHITE
        );
        DrawTexturePro(
            *middle,
            {0.0f, 0.0f, static_cast<float>(middle -> width), static_cast<float>(middle -> height)},
            {bounds.x + leftWidth, bounds.y, middleWidth, bounds.height},
            {0.0f, 0.0f}, 0.0f, WHITE
        );
        DrawTexturePro(
            *right,
            {0.0f, 0.0f, static_cast<float>(right -> width), static_cast<float>(right -> height)},
            {bounds.x + bounds.width - rightWidth, bounds.y, rightWidth, bounds.height},
            {0.0f, 0.0f}, 0.0f, WHITE
        );
    }
    else {
        DrawRectangleRounded(
            bounds, 0.25f, 8, hovered ? Color{113, 91, 148, 255} : Color{78, 63, 110, 255}
        );
    }

    if(textManager) {
        Rectangle textBounds = bounds;
        if(hovered) textBounds.y += 2.0f * getScale();

        textManager -> drawCenteredText(
            "Luckiest_Guy", label, textBounds, 22.0f * getScale(), 1.0f * getScale(),
            Color{255, 245, 201, 255}
        );
    }
}

void GameResultScreen :: handleInput(const RawInputEvent& inputEvent) {
    if(inputEvent.inputType != RawInputEvent :: InputType :: LEFT_MOUSE_CLICKED) {
        return;
    }

    if(data.wResult == WorldResult :: LOST &&
       loseAnimationTime < LOSE_ANIMATION_HOLD_TIME) {
        return;
    }

    if(CheckCollisionPointRec(inputEvent.position, getContinueButtonBounds())) {
        requestTransition(
            ScreenAction :: CLEAR_AND_PUSH,
            ScreenID :: MAIN_MENU
        );
    }
}
