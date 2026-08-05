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

    std::unique_ptr<Screen> createScreen(ScreenID id);
    void processTransition();

public:
    // ScreenManager behaves like a stack: screens can be pushed, popped or replaced.
    ScreenManager(int screenWidth, int screenHeight, AssetManager* assetManager);

    void push(ScreenID id);
    void pop();
    void replace(ScreenID id);
    void clearAndPush(ScreenID id);

    void handleInput(const RawInputEvent& inputEvent);
    void update(float dt);
    void draw();

    Screen* top();
    const Screen* top() const;
    bool empty() const;
};

#endif
