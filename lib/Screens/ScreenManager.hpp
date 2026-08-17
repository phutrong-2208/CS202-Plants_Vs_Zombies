#ifndef SCREEN_MANAGER_HPP
#define SCREEN_MANAGER_HPP

#include "Common.hpp"
#include "Screens/Screen.hpp"

class ScreenManager {
private:
    std::vector<std::unique_ptr<Screen>> screenStorage;
    int screenWidth = 0;
    int screenHeight = 0;
    AssetManager* assetManager = nullptr;
    UserProfileManager* userProfileManager = nullptr;
    bool quitRequested = false;

    std::unique_ptr<Screen> createScreen(ScreenID id, const ScreenData& data);
    void processTransition();

public:
    // ScreenManager behaves like a stack: screens can be pushed, popped or replaced.
    ScreenManager(
        int screenWidth,
        int screenHeight,
        AssetManager* assetManager,
        UserProfileManager* userProfileManager
    );

    void push(ScreenID id, ScreenData data = {});
    void pop();
    void replace(ScreenID id, ScreenData data = {});
    void clearAndPush(ScreenID id, ScreenData data = {});

    void handleInput(const RawInputEvent& inputEvent);
    void update(float dt);
    void draw();

    Screen* top();
    const Screen* top() const;
    bool empty() const;
    bool isQuitRequested() const;
};

#endif
