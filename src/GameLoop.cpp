#include <GameLoop.hpp>

void GameLoop::initGame() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200, 900, "Plants Vs Zombies");
    SetTargetFPS(60);

    assetManager = std :: make_unique<AssetManager>();
    assetManager -> beginLoading();
    inputManager = std :: make_unique<InputManager>();
    userProfileManager = std :: make_unique<UserProfileManager>();
    screenManager = std :: make_unique<ScreenManager>(
        GetScreenWidth(),
        GetScreenHeight(),
        assetManager.get(),
        userProfileManager.get()
    );
    screenManager -> push(ScreenID :: LOAD_MENU);
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
        screenManager->handleInput(inputEvent);
    }

    screenManager->update(GetFrameTime());

    BeginDrawing();
        ClearBackground(BLACK);
        screenManager->draw();

        DrawFPS(10, 10);
    EndDrawing();
}

void GameLoop::closeGame() {
    screenManager.reset();
    userProfileManager.reset();
    inputManager.reset();
    assetManager.reset();

    CloseWindow();
}
