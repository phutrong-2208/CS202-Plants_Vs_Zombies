#ifndef GAMEPLAY_SCREEN_HPP
#define GAMEPLAY_SCREEN_HPP

#include <Core/AssetManager.hpp>
#include <Screens/Screen.hpp>
#include <Worlds/World.hpp>
#include <UI/ChoosePlants.hpp>
#include <UI/SeedBank.hpp>
#include <UI/ConveyorBelt.hpp>
#include <UI/WaveHUD.hpp>
#include <UI/EndlessHUD.hpp>
#include <Gameplay/Endless/EndlessController.hpp>
#include <Gameplay/AI/PlantAIController.hpp>

class GameplayScreen : public Screen {
private:
    std::unique_ptr<World> world = nullptr;
    ChoosePlants choosePlants;
    SeedBank seedBank;
    ConveyorBelt conveyorBelt;
    PlantAIController plantAI;
    WaveHUD waveHUD;
    EndlessHUD endlessHUD;
    std::unique_ptr<EndlessController> endlessController;
    GameMode gameMode = GameMode::ADVENTURE;
    TextManager* textManager = nullptr;
    bool resultRequested = false;
    bool aiPvZMode = false;

    // Tracks previous phase to detect WAVE_RUNNING transitions (reset world result once)
    EndlessController :: Phase prevEndlessPhase = EndlessController :: Phase :: BETWEEN_WAVES;
    int screenWidth  = 800;
    int screenHeight = 600;

    TexturePackage* shovelPackage = nullptr;
    bool isShovelSelected = false;

    void drawSunHUD() const;
    void drawAIPvZCurrencyHUD() const;
    void drawShovel() const;
    Rectangle getShovelBounds() const;
    void drawPauseButton() const;
    Rectangle getPauseButtonBounds() const;
    void applyWinProgress(const ScreenData& resultData);
    void saveEndlessRecord(const ScreenData& resultData);

public:
    GameplayScreen(
        int screenWidth,
        int screenHeight,
        AssetManager* manager,
        LevelID levelID = {1, 1},
        UserProfile *user = nullptr,
        GameMode gameMode = GameMode::ADVENTURE
    );

    void update(float dt) override;
    void draw() override;
    void handleInput(const RawInputEvent& inputEvent) override;
};

#endif
