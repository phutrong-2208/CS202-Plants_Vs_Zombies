#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP

#include <Common.hpp>
#include <Core/InputManager.hpp>
#include <Core/AssetManager.hpp>
#include <Core/UserProfileManager.hpp>
#include <Screens/ScreenManager.hpp>

class GameLoop {
private:
    std::unique_ptr<AssetManager> assetManager = nullptr;
    std::unique_ptr<InputManager> inputManager = nullptr;
    std::unique_ptr<ScreenManager> screenManager = nullptr;
    std::unique_ptr<UserProfileManager> userProfileManager = nullptr;
public:
    void initGame();
    bool isRunning();
    void runGame();
    void closeGame();
};  


#endif
