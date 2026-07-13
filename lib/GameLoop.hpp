#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP

#include <Common.hpp>
#include <Core/InputManager.hpp>

#include <Worlds/World.hpp>

class GameLoop {
private:
    std::unique_ptr<World> world = nullptr;
    std::unique_ptr<InputManager> inputManager = nullptr;
public:
    void initGame();
    bool isRunning();
    void runGame();
    void closeGame();
};  


#endif