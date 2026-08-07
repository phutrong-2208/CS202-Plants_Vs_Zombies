#ifndef GAMEPLAY_SCREEN_HPP
#define GAMEPLAY_SCREEN_HPP

#include <Core/AssetManager.hpp>
#include <Screens/Screen.hpp>
#include <Worlds/World.hpp>
#include <UI/ChoosePlants.hpp>
#include <UI/SeedBank.hpp>
#include <UI/WaveHUD.hpp>

class GameplayScreen : public Screen {
private:
    std::unique_ptr<World> world = nullptr;
    ChoosePlants choosePlants;
    SeedBank seedBank;
    WaveHUD waveHUD;
    TextManager* textManager = nullptr;
    bool resultRequested = false;
    int screenWidth  = 800;
    int screenHeight = 600;

    void drawSunHUD() const;
    void drawPauseButton() const;
    Rectangle getPauseButtonBounds() const;
    void applyWinProgress(const ScreenData& resultData);

public:
    GameplayScreen(
        int screenWidth,
        int screenHeight,
        AssetManager* manager,
        LevelID levelID = {1, 1}
    );

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
