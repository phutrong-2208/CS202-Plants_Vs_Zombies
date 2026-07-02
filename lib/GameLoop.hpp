#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP

#include <Common.hpp>
#include <Core/InputManager.hpp>

#include <Worlds/World.hpp>

class GameLoop {
private:
    World* world = nullptr;
    InputManager* inputManager = nullptr;
public:
    void initGame();
    bool isRunning();
    void runGame();
    void closeGame();
    
    // void loadAssets();
};  


#endif