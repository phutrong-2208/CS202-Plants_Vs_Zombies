#include "Screens/MainMenuScreen.hpp"

namespace {
    constexpr float MAIN_MENU_WIDTH = 800.0f;
    constexpr float MAIN_MENU_HEIGHT = 600.0f;

    constexpr Vector2 WORLD_NUMBER_POSITION = {558.0f, 87.0f};
    constexpr Vector2 STAGE_NUMBER_POSITION = {580.0f, 88.0f};
    constexpr float LEVEL_DIGIT_WIDTH = 12.0f;
    constexpr float LEVEL_DIGIT_HEIGHT = 17.0f;
}

MainMenuScreen :: MainMenuScreen(
    int screenWidth,
    int screenHeight,
    AssetManager* manager
) : screenW(screenWidth), screenH(screenHeight) {
    setAssetManager(manager);

    if(!assetManager) {
        TraceLog(LOG_ERROR, "MainMenuScreen: AssetManager is null");
        return;
    }

    TextureManager* textureManager = assetManager -> getTextureManager();
    AnimationManager* animationManager = assetManager -> getAnimationManager();
    if(!textureManager || !animationManager) {
        TraceLog(LOG_ERROR, "MainMenuScreen: asset managers are unavailable");
        return;
    }

    TexturePackage* package = textureManager -> getPackage("MainMenu");
    ReanimParser* parser =
        animationManager -> getAnimationData("SelectorScreenAnim");

    if(!package || !parser) {
        TraceLog(LOG_ERROR, "MainMenuScreen: SelectorScreen assets are unavailable");
        return;
    }

    const float scale = std :: min(
        static_cast<float>(screenW) / MAIN_MENU_WIDTH,
        static_cast<float>(screenH) / MAIN_MENU_HEIGHT
    );

    if(!loadSceneAnimations(parser, package)) {
        TraceLog(LOG_ERROR, "MainMenuScreen: selector clips are unavailable");
        return;
    }

    levelNumbersTexture =
        package -> GetTexture("SELECTORSCREEN_LEVELNUMBERS");

    loadHoverButtons(package);
    loadFlowerLabels(package);
    animationReady = true;
}

float MainMenuScreen :: getMenuScale() const {
    return std :: min(
        static_cast<float>(screenW) / MAIN_MENU_WIDTH,
        static_cast<float>(screenH) / MAIN_MENU_HEIGHT
    );
}

Vector2 MainMenuScreen :: getMenuOffset() const {
    const float scale = getMenuScale();
    return {
        (static_cast<float>(screenW) - MAIN_MENU_WIDTH * scale) * 0.5f,
        (static_cast<float>(screenH) - MAIN_MENU_HEIGHT * scale) * 0.5f
    };
}

Rectangle MainMenuScreen :: toScreenBounds(Rectangle virtualBounds) const {
    const float scale = getMenuScale();
    const Vector2 offset = getMenuOffset();
    return {
        offset.x + virtualBounds.x * scale,
        offset.y + virtualBounds.y * scale,
        virtualBounds.width * scale,
        virtualBounds.height * scale
    };
}
//Control hover buttons
void MainMenuScreen :: loadHoverButtons(TexturePackage* package) {
    hoverButtons = {{
        {
            {25.5f, 126.8f, 291.0f, 53.6f},
            {25.5f, 126.8f},
            package -> GetTexture("SELECTORSCREEN_WOODSIGN2_PRESS"),
            MainMenuAction :: CHANGE_PROFILE
        },
        {
            {32.4f, 180.4f, 279.0f, 83.0f},
            {32.4f, 180.4f},
            package -> GetTexture("SELECTORSCREEN_WOODSIGN3_PRESS"), 
            MainMenuAction :: ZOMBATAR
        },
        {
            {405.0f, 65.0f, 331.0f, 108.1f},
            {405.0f, 65.0f},
            package -> GetTexture("SELECTORSCREEN_ADVENTURE_HIGHLIGHT"),
            MainMenuAction :: START_ADVENTURE
        },
        {
            {406.0f, 173.1f, 313.0f, 84.4f},
            {406.0f, 173.1f},
            package -> GetTexture("SELECTORSCREEN_SURVIVAL_HIGHLIGHT"),
            MainMenuAction :: SURVIVAL
        },
        {
            {410.0f, 257.5f, 286.0f, 70.5f},
            {410.0f, 257.5f},
            package -> GetTexture("SELECTORSCREEN_CHALLENGES_HIGHLIGHT"),
            MainMenuAction :: MINI_GAMES
        },
        {
            {413.0f, 328.0f, 266.0f, 123.0f},
            {413.0f, 328.0f},
            package -> GetTexture("SELECTORSCREEN_VASEBREAKER_HIGHLIGHT"),
            MainMenuAction :: PUZZLE
        }
    }};
}

void MainMenuScreen :: loadFlowerLabels(TexturePackage* package) {
    flowerLabels = {{
        {
            {558.0f, 481.0f, 81.0f, 31.0f},
            {558.0f, 481.0f},
            package -> GetTexture("SELECTORSCREEN_OPTIONS1"),
            package -> GetTexture("SELECTORSCREEN_OPTIONS2"), 
            MainMenuAction :: OPTION
        },
        {
            {654.0f, 501.0f, 48.0f, 22.0f},
            {654.0f, 501.0f},
            package -> GetTexture("SELECTORSCREEN_HELP1"),
            package -> GetTexture("SELECTORSCREEN_HELP2"),
            MainMenuAction :: HELP 
        },
        {
            {725.0f, 489.0f, 47.0f, 27.0f},
            {725.0f, 489.0f},
            package -> GetTexture("SELECTORSCREEN_QUIT1"),
            package -> GetTexture("SELECTORSCREEN_QUIT2"),
            MainMenuAction :: QUIT
        }
    }};
}

bool MainMenuScreen :: loadSceneAnimations(ReanimParser* parser, TexturePackage* package){
    const float scale = getMenuScale();
    const std :: vector<std :: string> dynamicTrackNames = {
        "Cloud1", "Cloud7", "Cloud2", "Cloud4", "Cloud6", "Cloud5",
        "leaf3", "leaf2", "leaf22", "leaf_SelectorScreen_Leaves",
        "leaf4", "leaf5", "leaf1"
    };

    skyAnimation.setAnimation(parser);
    skyAnimation.setTexturePackage(package);
    skyAnimation.setTextureScalar(scale);
    skyAnimation.showOnlyTracks({"SelectorScreen_BG"});
    if(!skyAnimation.playClip("sign")) return false;
    skyAnimation.setLoopToggle(false);

    mainAnimation.setAnimation(parser);
    mainAnimation.setTexturePackage(package);
    mainAnimation.setTextureScalar(scale);
    mainAnimation.hideTrack("SelectorScreen_BG");
    mainAnimation.hideTrack("woodsign1");
    mainAnimation.hideTrack("woodsign2");
    mainAnimation.hideTrack("woodsign3");

    for(const std :: string& trackName : dynamicTrackNames) {
        mainAnimation.hideTrack(trackName);
    }

    if(!mainAnimation.playClip("sign")) return false;
    mainAnimation.setLoopToggle(false);

    woodSignAnimation.setAnimation(parser);
    woodSignAnimation.setTexturePackage(package);
    woodSignAnimation.setTextureScalar(scale);
    woodSignAnimation.showOnlyTracks({"woodsign1", "woodsign2", "woodsign3"});
    if(!woodSignAnimation.playClip("sign")) return false;
    woodSignAnimation.setLoopToggle(false);

    struct DynamicClip {
        const char* clipName;
        std :: vector<std :: string> trackNames;
    };

    //drawing cloud
    const std :: array<DynamicClip, 7> clips = {{
        {"cloud1", {"Cloud1"}},
        {"cloud7", {"Cloud7"}},
        {"cloud2", {"Cloud2"}},
        {"cloud4", {"Cloud4"}},
        {"cloud6", {"Cloud6"}},
        {"cloud5", {"Cloud5"}},
        {"grass", {
            "leaf3", "leaf2", "leaf22", "leaf_SelectorScreen_Leaves",
            "leaf4", "leaf5", "leaf1"
        }}
    }};

    for(size_t index = 0; index < dynamicAnimations.size(); ++index) {
        ReanimInstance& animation = dynamicAnimations[index];
        animation.setAnimation(parser);
        animation.setTexturePackage(package);
        animation.setTextureScalar(scale);
        animation.showOnlyTracks(clips[index].trackNames);
        if(!animation.playClip(clips[index].clipName)) return false;
    }
    return true;
}

void MainMenuScreen :: drawLevelNumber(
    int value,
    Vector2 position,
    float scale
) const {
    if(levelNumbersTexture == nullptr || value < 0) return;

    const std :: string digits = std :: to_string(value);
    for(size_t index = 0; index < digits.size(); ++index) {
        const int digit = digits[index] - '0';
        if(digit < 0 || digit > 9) continue;

        DrawTexturePro(
            *levelNumbersTexture,
            {
                digit * LEVEL_DIGIT_WIDTH,
                0.0f,
                LEVEL_DIGIT_WIDTH,
                LEVEL_DIGIT_HEIGHT
            },
            {
                position.x + index * LEVEL_DIGIT_WIDTH * scale,
                position.y,
                LEVEL_DIGIT_WIDTH * scale,
                LEVEL_DIGIT_HEIGHT * scale
            },
            {0.0f, 0.0f},
            0.0f,
            WHITE
        );
    }
}

void MainMenuScreen :: draw(void){
    if(!animationReady) return;

    const float scale = getMenuScale();
    const Vector2 offset = getMenuOffset();

    const Rectangle menuBounds = {
        offset.x,
        offset.y,
        MAIN_MENU_WIDTH * scale,
        MAIN_MENU_HEIGHT * scale
    };

    skyAnimation.draw(menuBounds);
    for(size_t index = 0; index < 6; ++index) {
        dynamicAnimations[index].draw(menuBounds);
    }
    mainAnimation.draw(menuBounds);
    dynamicAnimations[6].draw(menuBounds); 
    woodSignAnimation.draw(menuBounds);

    for(const MainMenuHoverButton& button : hoverButtons) {
        if(!button.hovered || button.highlightTexture == nullptr) continue;

        DrawTextureEx(
            *button.highlightTexture,
            {
                offset.x + button.texturePosition.x * scale,
                offset.y + button.texturePosition.y * scale
            },
            0.0f,
            scale,
            WHITE
        );
    }

    // Hover redraws the stone texture, so digits must be drawn on top of it.
    drawLevelNumber(
        id.world,
        {
            offset.x + WORLD_NUMBER_POSITION.x * scale,
            offset.y + WORLD_NUMBER_POSITION.y * scale
        },
        scale
    );
    drawLevelNumber(
        id.stage,
        {
            offset.x + STAGE_NUMBER_POSITION.x * scale,
            offset.y + STAGE_NUMBER_POSITION.y * scale
        },
        scale
    );

    for(const MainMenuFlowerLabel& label : flowerLabels) {
        Texture2D* texture = label.hovered && label.hoverTexture != nullptr
                ? label.hoverTexture : label.normalTexture;
        if(texture == nullptr) continue;

        DrawTextureEx(
            *texture,{
                offset.x + label.texturePosition.x * scale,
                offset.y + label.texturePosition.y * scale
            },
            0.0f, scale, WHITE
        );
    }
}

void MainMenuScreen :: update(float dt){
    if(!animationReady) return;

    skyAnimation.updateTime(dt);
    mainAnimation.updateTime(dt);
    woodSignAnimation.updateTime(dt);
    for(ReanimInstance& animation : dynamicAnimations) {
        animation.updateTime(dt);
    }

    const Vector2 mousePosition = GetMousePosition();
    for(MainMenuHoverButton& button : hoverButtons) {
        button.hovered = button.enabled and CheckCollisionPointRec(
            mousePosition,
            toScreenBounds(button.bounds)
        );
    }
    for(MainMenuFlowerLabel& label : flowerLabels) {
        label.hovered = label.enabled and CheckCollisionPointRec(
            mousePosition,
            toScreenBounds(label.bounds)
        );
    }
}

void MainMenuScreen :: executeAction(MainMenuAction target){
    switch(target){
        case MainMenuAction :: START_ADVENTURE:
            requestTransition(ScreenAction :: REPLACE, ScreenID :: GAME_PLAY); break;
        case MainMenuAction :: OPTION:
            requestTransition(ScreenAction :: PUSH, ScreenID :: PAUSE_MENU); break;
    }
}

void MainMenuScreen :: handleInput(const RawInputEvent& inputEvent){
    if (inputEvent.inputType != RawInputEvent::InputType::LEFT_MOUSE_CLICKED) {
        return;
    }

    constexpr std::size_t START_ADVENTURE_INDEX = 2;
    if (CheckCollisionPointRec(
            inputEvent.position,
            toScreenBounds(hoverButtons[START_ADVENTURE_INDEX].bounds)
        )) {
        requestTransition(ScreenAction::REPLACE, ScreenID::GAME_PLAY);
    }
}
