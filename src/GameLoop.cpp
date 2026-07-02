#include <GameLoop.hpp>


// void GameLoop::loadAssets(void) {
    
// }

void GameLoop::initGame() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200, 900, "Plants Vs Zombies");
    SetTargetFPS(60);

    world = new World(GetScreenWidth(), GetScreenHeight());
    inputManager = new InputManager();

    // loadAssets();
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

        if (inputEvent.inputType == RawInputEvent::InputType::LEFT_MOUSE_CLICKED) {
            world -> onMouseClick(inputEvent.position);
        }
    }

    world -> update(GetFrameTime());

    BeginDrawing();
        ClearBackground(BLACK);
        world -> draw();

        DrawFPS(10, 10);
    EndDrawing();
}

void GameLoop::closeGame() {
    delete world;
    delete inputManager;

    CloseWindow();
}