#include <GameLoop.hpp>


void GameLoop::loadAssets(void) {
    std :: string path = GetApplicationDirectory();
    std :: string dir = path + "../assets/texture/Plants/SunFlower";
    std :: unique_ptr<TexturePackage> SunFlower = std :: make_unique<TexturePackage>();

    SunFlower -> AddTexture("SUNFLOWER_BLINK1",       dir + "/SunFlower_blink1.png");
    SunFlower -> AddTexture("SUNFLOWER_BLINK2",       dir + "/SunFlower_blink2.png");
    SunFlower -> AddTexture("SUNFLOWER_BOTTOMPETALS", dir + "/SunFlower_bottompetals.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD",         dir + "/SunFlower_head.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING1",   dir + "/SunFlower_head_sing1.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING2",   dir + "/SunFlower_head_sing2.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING3",   dir + "/SunFlower_head_sing3.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING4",   dir + "/SunFlower_head_sing4.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_SING5",   dir + "/SunFlower_head_sing5.png");
    SunFlower -> AddTexture("SUNFLOWER_HEAD_WINK",    dir + "/SunFlower_head_wink.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL1",   dir + "/SunFlower_leftpetal1.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL2",   dir + "/SunFlower_leftpetal2.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL3",   dir + "/SunFlower_leftpetal3.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL4",   dir + "/SunFlower_leftpetal4.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL5",   dir + "/SunFlower_leftpetal5.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL6",   dir + "/SunFlower_leftpetal6.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL7",   dir + "/SunFlower_leftpetal7.png");
    SunFlower -> AddTexture("SUNFLOWER_LEFTPETAL8",   dir + "/SunFlower_leftpetal8.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL1",  dir + "/SunFlower_rightpetal1.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL2",  dir + "/SunFlower_rightpetal2.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL3",  dir + "/SunFlower_rightpetal3.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL4",  dir + "/SunFlower_rightpetal4.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL5",  dir + "/SunFlower_rightpetal5.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL6",  dir + "/SunFlower_rightpetal6.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL7",  dir + "/SunFlower_rightpetal7.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL8",  dir + "/SunFlower_rightpetal8.png");
    SunFlower -> AddTexture("SUNFLOWER_RIGHTPETAL9",  dir + "/SunFlower_rightpetal9.png");
    SunFlower -> AddTexture("SUNFLOWER_DOUBLE_PETALS", dir + "/SunFlower_double_petals.png");
    SunFlower -> AddTexture("SUNFLOWER_TOPPETALS",    dir + "/SunFlower_toppetals.png");

    textureManager -> addPackage("SunFlower", std :: move(SunFlower));

}

void GameLoop::initGame() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1200, 900, "Plants Vs Zombies");
    SetTargetFPS(60);

    world = new World(GetScreenWidth(), GetScreenHeight());
    inputManager = new InputManager();
    textureManager = new TextureManager();
    animationManager = new AnimationManager();

    loadAssets();


    auto parser = std::make_unique<ReanimParser>();
    parser->loadFromFile("../assets/texture/Plants/SunFlower/SunFlower.reanim");
    animationManager->addClip("idle", parser.release());
    animationManager->play("idle");
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
    animationManager->update(GetFrameTime());

    BeginDrawing();
    ClearBackground(BLACK);
    // world -> draw();
    animationManager->draw({400, 300}, *textureManager, "SunFlower", 200.0f);
    DrawFPS(10, 10);
    EndDrawing();
}

void GameLoop::closeGame() {
    delete world;
    delete inputManager;
    delete textureManager;
    delete animationManager;

    CloseWindow();
}