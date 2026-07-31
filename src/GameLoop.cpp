#include <GameLoop.hpp>
#include <Screens/GameplayScreen.hpp>
#include <Screens/MainMenuScreen.hpp>


void GameLoop::initGame() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200, 900, "Plants Vs Zombies");
    SetTargetFPS(60);

    assetManager = std::make_unique<AssetManager>();
    activeScreen = std::make_unique<MainMenuScreen>(GetScreenWidth(), GetScreenHeight(), assetManager.get()); //will be replaced later 
    inputManager = std::make_unique<InputManager>();
}


bool GameLoop::isRunning() {
    return !WindowShouldClose();
}

void GameLoop::runGame() {
    // Prevent crashing when letting the computer in sleep state
    if (IsWindowMinimized()) {
        WaitTime(1); return;
    }
    
    inputManager -> update();
    while (inputManager -> hasEvents()) {
        RawInputEvent inputEvent = inputManager -> pollEvent();
        if (activeScreen) {
            activeScreen -> handleInput(inputEvent);
        }
    }

    if (activeScreen) {
        activeScreen -> update(GetFrameTime());
    }

    BeginDrawing();
        ClearBackground(BLACK);
        if (activeScreen) {
            activeScreen -> draw();
        }

        DrawFPS(10, 10);
    EndDrawing();
}

void GameLoop::closeGame() {
    activeScreen.reset();
    inputManager.reset();

    CloseWindow();
}
