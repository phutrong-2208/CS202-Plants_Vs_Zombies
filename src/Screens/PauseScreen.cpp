#include <Screens/PauseScreen.hpp>
#include <algorithm>

namespace {
    constexpr float VIRTUAL_WIDTH = 800.0f;
    constexpr float VIRTUAL_HEIGHT = 600.0f;

    constexpr Rectangle PANEL_BOUNDS       = {188.5f, 51.0f, 423.0f, 498.0f};

    constexpr Rectangle MUSIC_LABEL_BOUNDS = {240.0f, 192.0f, 130.0f, 26.0f};
    constexpr Rectangle MUSIC_SLOT_BOUNDS  = {380.0f, 200.0f, 140.0f, 10.0f};
    constexpr Rectangle MUSIC_HIT_BOUNDS   = {370.0f, 185.0f, 160.0f, 36.0f};

    constexpr Rectangle SOUND_LABEL_BOUNDS = {240.0f, 252.0f, 130.0f, 26.0f};
    constexpr Rectangle SOUND_SLOT_BOUNDS  = {380.0f, 260.0f, 140.0f, 10.0f};
    constexpr Rectangle SOUND_HIT_BOUNDS   = {370.0f, 245.0f, 160.0f, 36.0f};

    constexpr Rectangle MAIN_MENU_BOUNDS   = {270.0f, 345.0f, 260.0f, 45.0f};
    constexpr Rectangle RESUME_BOUNDS      = {220.0f, 420.0f, 360.0f, 95.0f};

    constexpr Color MENU_TEXT_COLOR   = {55, 52, 78, 255};
    constexpr Color LABEL_TEXT_COLOR  = {245, 240, 220, 255};
    constexpr Color LABEL_SHADOW_COLOR = {25, 25, 40, 255};
    constexpr Color OK_GREEN_COLOR    = {115, 230, 45, 255};
}

PauseScreen :: PauseScreen(int screenWidth, int screenHeight, AssetManager* manager, const ScreenData& data) : 
screenWidth(screenWidth), screenHeight(screenHeight), isSettingsMenu(data.isSettingsMenu) {
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
    if (package != nullptr) {
        menuBackground = package -> GetTexture("OPTIONS_MENUBACK");
        buttonNormal   = package -> GetTexture("OPTIONS_BACKTOGAMEBUTTON0");
        buttonHover    = package -> GetTexture("OPTIONS_BACKTOGAMEBUTTON2");
        sliderSlot     = package -> GetTexture("OPTIONS_SLIDERSLOT");
        sliderKnob     = package -> GetTexture("OPTIONS_SLIDERKNOB2");
    }

    if (!sliderSlot || !sliderKnob) {
        TexturePackage* sliderPkg = textureManager->getPackage("Slider");
        if (sliderPkg != nullptr) {
            if (!sliderSlot) sliderSlot = sliderPkg->GetTexture("OPTIONS_SLIDERSLOT");
            if (!sliderKnob) sliderKnob = sliderPkg->GetTexture("OPTIONS_SLIDERKNOB2");
        }
    }
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

void PauseScreen::updateSliderFromMouse(bool isMusic, float mouseVirtualX) {
    const Rectangle slotBounds = isMusic ? MUSIC_SLOT_BOUNDS : SOUND_SLOT_BOUNDS;
    float value = std::clamp((mouseVirtualX - slotBounds.x) / slotBounds.width, 0.0f, 1.0f);

    if (isMusic) {
        if (assetManager && assetManager->getMusicManager()) {
            MusicManager* mm = assetManager->getMusicManager();
            mm->setMasterVolume(value);
            if (value > 0.001f && mm->isMuted()) {
                mm->setMuted(false);
            }
        }
    } else {
        if (assetManager && assetManager->getSoundManager()) {
            SoundManager* sm = assetManager->getSoundManager();
            sm->setMasterVolume(value);
            if (value > 0.001f && sm->isMuted()) {
                sm->setMuted(false);
            }
        }
    }
}

void PauseScreen::update(float) {
    const Vector2 mousePosition = GetMousePosition();
    const float scale = getScale();
    const Vector2 offset = getOffset();
    const float mouseVirtualX = (mousePosition.x - offset.x) / scale;

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        draggingMusic = false;
        draggingSound = false;
    }

    if (draggingMusic) {
        updateSliderFromMouse(true, mouseVirtualX);
    } else if (draggingSound) {
        updateSliderFromMouse(false, mouseVirtualX);
    }

    musicSliderHovered = CheckCollisionPointRec(mousePosition, toScreenBounds(MUSIC_HIT_BOUNDS));
    soundSliderHovered = CheckCollisionPointRec(mousePosition, toScreenBounds(SOUND_HIT_BOUNDS));
    resumeHovered      = CheckCollisionPointRec(mousePosition, toScreenBounds(RESUME_BOUNDS));
    if (!isSettingsMenu) {
        mainMenuHovered = CheckCollisionPointRec(mousePosition, toScreenBounds(MAIN_MENU_BOUNDS));
    }
}

void PauseScreen :: drawButton(Rectangle bounds, const char* text, bool hovered, Color textColor) const {
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
        textManager -> drawCenteredText("LUCKIESTGUY-REGULAR", text, screenBounds, 25.0f * getScale(), getScale(), textColor);
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

    const float scale = getScale();

    // 1. Music Slider Label
    if (textManager != nullptr) {
        Rectangle labelBounds = toScreenBounds(MUSIC_LABEL_BOUNDS);
        // Shadow
        textManager->drawCenteredText(
            "LUCKIESTGUY-REGULAR", "Music",
            {labelBounds.x + 2.0f * scale, labelBounds.y + 2.0f * scale, labelBounds.width, labelBounds.height},
            24.0f * scale, scale, LABEL_SHADOW_COLOR
        );
        // Bright text
        textManager->drawCenteredText(
            "LUCKIESTGUY-REGULAR", "Music",
            labelBounds,
            24.0f * scale, scale, LABEL_TEXT_COLOR
        );
    }

    const Rectangle screenMusicSlot = toScreenBounds(MUSIC_SLOT_BOUNDS);
    if (sliderSlot != nullptr) {
        DrawTexturePro(
            *sliderSlot,
            {0.0f, 0.0f, static_cast<float>(sliderSlot->width), static_cast<float>(sliderSlot->height)},
            screenMusicSlot, {0.0f, 0.0f}, 0.0f, WHITE
        );
    } else {
        DrawRectangleRec(screenMusicSlot, Color{40, 42, 60, 255});
    }

    float musicVol = (assetManager && assetManager->getMusicManager()) ?
        (assetManager->getMusicManager()->isMuted() ? 0.0f : assetManager->getMusicManager()->getMasterVolume()) : 0.5f;

    float knobW = (sliderKnob ? static_cast<float>(sliderKnob->width) : 22.0f) * scale;
    float knobH = (sliderKnob ? static_cast<float>(sliderKnob->height) : 29.0f) * scale;
    float musicKnobX = screenMusicSlot.x + (screenMusicSlot.width - knobW) * musicVol;
    float musicKnobY = screenMusicSlot.y + (screenMusicSlot.height - knobH) * 0.5f - (2.0f * scale);
    Rectangle screenMusicKnob = {musicKnobX, musicKnobY, knobW, knobH};

    if (sliderKnob != nullptr) {
        DrawTexturePro(
            *sliderKnob,
            {0.0f, 0.0f, static_cast<float>(sliderKnob->width), static_cast<float>(sliderKnob->height)},
            screenMusicKnob, {0.0f, 0.0f}, 0.0f,
            (draggingMusic || musicSliderHovered) ? Color{255, 255, 200, 255} : WHITE
        );
    } else {
        DrawRectangleRec(screenMusicKnob, WHITE);
    }

    // 2. Sound FX Slider Label
    if (textManager != nullptr) {
        Rectangle labelBounds = toScreenBounds(SOUND_LABEL_BOUNDS);
        // Shadow
        textManager->drawCenteredText(
            "LUCKIESTGUY-REGULAR", "Sound FX",
            {labelBounds.x + 2.0f * scale, labelBounds.y + 2.0f * scale, labelBounds.width, labelBounds.height},
            24.0f * scale, scale, LABEL_SHADOW_COLOR
        );
        // Bright text
        textManager->drawCenteredText(
            "LUCKIESTGUY-REGULAR", "Sound FX",
            labelBounds,
            24.0f * scale, scale, LABEL_TEXT_COLOR
        );
    }

    const Rectangle screenSoundSlot = toScreenBounds(SOUND_SLOT_BOUNDS);
    if (sliderSlot != nullptr) {
        DrawTexturePro(
            *sliderSlot,
            {0.0f, 0.0f, static_cast<float>(sliderSlot->width), static_cast<float>(sliderSlot->height)},
            screenSoundSlot, {0.0f, 0.0f}, 0.0f, WHITE
        );
    } else {
        DrawRectangleRec(screenSoundSlot, Color{40, 42, 60, 255});
    }

    float soundVol = (assetManager && assetManager->getSoundManager()) ?
        (assetManager->getSoundManager()->isMuted() ? 0.0f : assetManager->getSoundManager()->getMasterVolume()) : 1.0f;

    float soundKnobX = screenSoundSlot.x + (screenSoundSlot.width - knobW) * soundVol;
    float soundKnobY = screenSoundSlot.y + (screenSoundSlot.height - knobH) * 0.5f - (2.0f * scale);
    Rectangle screenSoundKnob = {soundKnobX, soundKnobY, knobW, knobH};

    if (sliderKnob != nullptr) {
        DrawTexturePro(
            *sliderKnob,
            {0.0f, 0.0f, static_cast<float>(sliderKnob->width), static_cast<float>(sliderKnob->height)},
            screenSoundKnob, {0.0f, 0.0f}, 0.0f,
            (draggingSound || soundSliderHovered) ? Color{255, 255, 200, 255} : WHITE
        );
    } else {
        DrawRectangleRec(screenSoundKnob, WHITE);
    }

    // Buttons
    if (isSettingsMenu) {
        drawButton(RESUME_BOUNDS, "OK", resumeHovered, OK_GREEN_COLOR);
    } else {
        drawButton(MAIN_MENU_BOUNDS, "MAIN MENU", mainMenuHovered, MENU_TEXT_COLOR);
        drawButton(RESUME_BOUNDS, "BACK TO GAME", resumeHovered, MENU_TEXT_COLOR);
    }
}

void PauseScreen :: handleInput(const RawInputEvent& inputEvent) {
    if (inputEvent.inputType == RawInputEvent :: InputType :: LEFT_MOUSE_RELEASED) {
        draggingMusic = false;
        draggingSound = false;
        return;
    }

    if (inputEvent.inputType != RawInputEvent :: InputType :: LEFT_MOUSE_CLICKED) {
        return;
    }

    const float scale = getScale();
    const Vector2 offset = getOffset();
    const float mouseVirtualX = (inputEvent.position.x - offset.x) / scale;

    if (CheckCollisionPointRec(inputEvent.position, toScreenBounds(MUSIC_HIT_BOUNDS))) {
        draggingMusic = true;
        updateSliderFromMouse(true, mouseVirtualX);
        return;
    }

    if (CheckCollisionPointRec(inputEvent.position, toScreenBounds(SOUND_HIT_BOUNDS))) {
        draggingSound = true;
        updateSliderFromMouse(false, mouseVirtualX);
        if (assetManager && assetManager->getSoundManager()) {
            assetManager->getSoundManager()->play("BUTTONCLICK", 1.0f);
        }
        return;
    }

    if (CheckCollisionPointRec(inputEvent.position, toScreenBounds(RESUME_BOUNDS))) {
        if (assetManager && assetManager->getSoundManager()) {
            assetManager->getSoundManager()->play("BUTTONCLICK", 1.0f);
        }
        requestTransition(ScreenAction :: POP);
        return;
    }

    if (!isSettingsMenu && CheckCollisionPointRec(inputEvent.position, toScreenBounds(MAIN_MENU_BOUNDS))) {
        if (assetManager && assetManager->getSoundManager()) {
            assetManager->getSoundManager()->play("BUTTONCLICK", 1.0f);
        }
        requestTransition(ScreenAction :: CLEAR_AND_PUSH, ScreenID :: MAIN_MENU);
    }
}
