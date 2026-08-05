#include <Screens/PauseScreen.hpp>
#include <algorithm>

namespace {
    constexpr float VIRTUAL_WIDTH = 800.0f;
    constexpr float VIRTUAL_HEIGHT = 600.0f;

    constexpr Rectangle PANEL_BOUNDS = {188.5f, 51.0f, 423.0f, 498.0f};
    constexpr Rectangle TITLE_BOUNDS = {240.0f, 135.0f, 320.0f, 55.0f};
    constexpr Rectangle MESSAGE_BOUNDS = {245.0f, 210.0f, 310.0f, 65.0f};
    constexpr Rectangle MAIN_MENU_BOUNDS = {250.0f, 325.0f, 300.0f, 76.0f};
    constexpr Rectangle RESUME_BOUNDS = {220.0f, 415.0f, 360.0f, 100.0f};
}

PauseScreen :: PauseScreen(int screenWidth, int screenHeight, AssetManager* manager) : 
screenWidth(screenWidth), screenHeight(screenHeight) {
    setAssetManager(manager);
    if (assetManager == nullptr) {
        TraceLog(LOG_ERROR, "PauseScreen: AssetManager is null");
        return;
    }

    TextureManager* textureManager = assetManager->getTextureManager();
    textManager = assetManager->getTextManager();
    if (textureManager == nullptr) {
        TraceLog(LOG_ERROR, "PauseScreen: TextureManager is unavailable");
        return;
    }

    TexturePackage* package = textureManager->getPackage("PauseMenu");
    if (package == nullptr) {
        TraceLog(LOG_ERROR, "PauseScreen: PauseMenu package is unavailable");
        return;
    }

    menuBackground = package -> GetTexture("OPTIONS_MENUBACK");
    buttonNormal = package -> GetTexture("OPTIONS_BACKTOGAMEBUTTON0");
    buttonHover = package -> GetTexture("OPTIONS_BACKTOGAMEBUTTON2");
}

float PauseScreen :: getScale() const {
    return std::min(
        static_cast<float>(screenWidth) / VIRTUAL_WIDTH,
        static_cast<float>(screenHeight) / VIRTUAL_HEIGHT
    );
}

Vector2 PauseScreen :: getOffset() const {
    const float scale = getScale();
    return {
        (static_cast<float>(screenWidth) - VIRTUAL_WIDTH * scale) * 0.5f,
        (static_cast<float>(screenHeight) - VIRTUAL_HEIGHT * scale) * 0.5f
    };
}

Rectangle PauseScreen::toScreenBounds(Rectangle virtualBounds) const {
    const float scale = getScale();
    const Vector2 offset = getOffset();
    return {
        offset.x + virtualBounds.x * scale,
        offset.y + virtualBounds.y * scale,
        virtualBounds.width * scale,
        virtualBounds.height * scale
    };
}

void PauseScreen::update(float) {
    const Vector2 mousePosition = GetMousePosition();
    resumeHovered = CheckCollisionPointRec(
        mousePosition,
        toScreenBounds(RESUME_BOUNDS)
    );
    mainMenuHovered = CheckCollisionPointRec(
        mousePosition,
        toScreenBounds(MAIN_MENU_BOUNDS)
    );
}

void PauseScreen :: drawButton(Rectangle bounds, const char* text, bool hovered) const {
    const Rectangle screenBounds = toScreenBounds(bounds);
    Texture2D* texture = hovered && buttonHover != nullptr ? buttonHover : buttonNormal;

    if (texture != nullptr) {
        DrawTexturePro(*texture,
            {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)},
            screenBounds, {0.0f, 0.0f}, 0.0f, WHITE
        );
    } else {
        DrawRectangleRounded(screenBounds, 0.2f, 8, hovered ? Color{105, 112, 145, 255} : Color{70, 76, 105, 255});
    }

    if (textManager != nullptr) {
        textManager -> drawCenteredText(
            "LUCKIESTGUY-REGULAR", text, screenBounds, 25.0f * getScale(), getScale(), hovered ? Color{255, 245, 150, 255} : WHITE
        );
    }
}

void PauseScreen::draw() {
    DrawRectangle(
        0, 0, screenWidth, screenHeight, Color{0, 0, 0, 135}
    );

    if (menuBackground != nullptr) {
        DrawTexturePro(
            *menuBackground,
            {0.0f, 0.0f, static_cast<float>(menuBackground->width), static_cast<float>(menuBackground->height)},
            toScreenBounds(PANEL_BOUNDS), {0.0f, 0.0f}, 0.0f, WHITE
        );
    }

    if (textManager != nullptr) {
        const float scale = getScale();
        textManager -> drawCenteredText(
            "LUCKIESTGUY-REGULAR", "GAME PAUSED", toScreenBounds(TITLE_BOUNDS),
            34.0f * scale,
            scale,
            Color{255, 225, 120, 255}
        );
        textManager -> drawCenteredText(
            "LUCKIESTGUY-REGULAR",
            "THE ZOMBIES ARE WAITING...",
            toScreenBounds(MESSAGE_BOUNDS),
            18.0f * scale,
            scale,
            Color{205, 210, 225, 255}
        );
    }

    drawButton(MAIN_MENU_BOUNDS, "MAIN MENU", mainMenuHovered);
    drawButton(RESUME_BOUNDS, "BACK TO GAME", resumeHovered);
}

void PauseScreen :: handleInput(const RawInputEvent& inputEvent) {
    if (inputEvent.inputType != RawInputEvent :: InputType :: LEFT_MOUSE_CLICKED) {
        return;
    }

    if (CheckCollisionPointRec(inputEvent.position, toScreenBounds(RESUME_BOUNDS))) {
        requestTransition(ScreenAction :: POP);
        return;
    }

    if (CheckCollisionPointRec(inputEvent.position, toScreenBounds(MAIN_MENU_BOUNDS))) {
        requestTransition(ScreenAction :: CLEAR_AND_PUSH, ScreenID :: MAIN_MENU);
    }
}
