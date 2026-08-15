#include "Screens/ScreenManager.hpp"
#include "Screens/GameplayScreen.hpp"
#include "Screens/GameResultScreen.hpp"
#include "Screens/LoadScreen.hpp"
#include "Screens/MainMenuScreen.hpp"
#include "Screens/PauseScreen.hpp"
#include "Screens/UserProfileScreen.hpp"

ScreenManager::ScreenManager(
    int width,
    int height,
    AssetManager* assets,
    UserProfileManager* profiles
) : screenWidth(width), screenHeight(height), assetManager(assets),
    userProfileManager(profiles) {}

std :: unique_ptr<Screen> ScreenManager :: createScreen(ScreenID id, const ScreenData& data) {
    std :: unique_ptr<Screen> screen;
    switch (id) {
        case ScreenID :: GAME_PLAY:
            screen = std :: make_unique<GameplayScreen>(
                screenWidth, screenHeight, assetManager, data.levelID, 
                userProfileManager -> getActiveProfile(),
                data.gameMode
            );
            break;

        case ScreenID :: GAME_RESULT:
            screen = std :: make_unique<GameResultScreen>(
                screenWidth, screenHeight, assetManager, data
            );
            break;

        case ScreenID :: MAIN_MENU:
            screen = std :: make_unique<MainMenuScreen>(
                screenWidth, screenHeight, assetManager
            );
            break;

        case ScreenID :: USER_PROFILE:
            screen = std :: make_unique<UserProfileScreen>(
                screenWidth, screenHeight, assetManager
            );
            break;

        case ScreenID :: LOAD_MENU:
            screen = std :: make_unique<LoadScreen>(
                screenWidth, screenHeight, assetManager
            );
            break;

        case ScreenID :: PAUSE_MENU:
            screen = std :: make_unique<PauseScreen>(
                screenWidth, screenHeight, assetManager
            );
            break;

        default:
            break;
    }

    if(screen) screen -> setUserProfileManager(userProfileManager);
    return screen;
}

void ScreenManager :: push(ScreenID id, ScreenData data) {
    auto screen = createScreen(id, data);
    if (screen) {
        screenStorage.push_back(std :: move(screen));
    }
}

void ScreenManager :: pop() {
    if (!screenStorage.empty()) {
        screenStorage.pop_back();
    }
}

void ScreenManager :: replace(ScreenID id, ScreenData data) {
    auto screen = createScreen(id, data);
    if (!screen) return;

    if (!screenStorage.empty()) {
        screenStorage.pop_back();
    }
    screenStorage.push_back(std :: move(screen));
}

void ScreenManager::clearAndPush(ScreenID id, ScreenData data) {
    auto screen = createScreen(id, data);
    if (!screen) return;

    screenStorage.clear();
    screenStorage.push_back(std::move(screen));
}

void ScreenManager :: processTransition() {
    Screen* screen = top();
    if (screen == nullptr) return;

    const ScreenTransition transition = screen->consumeTransition();
    switch (transition.action) {
        case ScreenAction :: PUSH:
            push(transition.target, transition.data);
            break;

        case ScreenAction :: POP:
            pop();
            break;

        case ScreenAction :: REPLACE:
            replace(transition.target, transition.data);
            break;

        case ScreenAction :: CLEAR_AND_PUSH:
            clearAndPush(transition.target, transition.data);
            break;

        case ScreenAction :: NONE:
        default:
            break;
    }
}

void ScreenManager::handleInput(const RawInputEvent& inputEvent) {
    if (Screen* screen = top()) {
        screen -> handleInput(inputEvent);
    }
    processTransition();
}

void ScreenManager :: update(float dt) {
    if (Screen* screen = top()) {
        screen -> update(dt);
    }
    processTransition();
}

void ScreenManager :: draw() {
    for (const auto& screen : screenStorage) {
        screen -> draw();
    }
}

Screen* ScreenManager :: top() {
    return screenStorage.empty() ? nullptr : screenStorage.back().get();
}

const Screen* ScreenManager :: top() const {
    return screenStorage.empty() ? nullptr : screenStorage.back().get();
}

bool ScreenManager :: empty() const {
    return screenStorage.empty();
}
