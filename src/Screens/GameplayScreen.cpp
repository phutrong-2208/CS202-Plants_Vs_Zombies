#include <Screens/GameplayScreen.hpp>
#include <Core/UserProfileManager.hpp>

#include <filesystem>

GameplayScreen :: GameplayScreen(int screenWidth, int screenHeight, AssetManager* manager, LevelID levelID, UserProfile* user) {
    setAssetManager(manager);
    world = std::make_unique<World>(
        screenWidth, screenHeight, assetManager, levelID
    );
    textManager = assetManager -> getTextManager();
    this -> screenWidth  = screenWidth;
    this -> screenHeight = screenHeight;

    waveHUD.setTexturePackage(assetManager->getTextureManager()->getPackage("ZombiePhase"));

    seedBank.setTexturePackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    seedBank.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
    seedBank.setTextManager(assetManager -> getTextManager());

    choosePlants.setChooserPackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    choosePlants.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
    choosePlants.setTextManager(assetManager -> getTextManager());
    choosePlants.setSunCosts(world->getAllSunCosts());
    choosePlants.setAvailablePlants(getAllPlantTypes());
    choosePlants.setMaxSlots(6);
    choosePlants.setSeedBank(&seedBank);
    choosePlants.setUnlockedPlants(user -> getUnlockedPlants());
}

void GameplayScreen :: update(float dt) {
    if (world) {
        world -> update(dt);

        if(!resultRequested && world -> getResult() != WorldResult :: RUNNING) {
            resultRequested = true;

            ScreenData resultData;
            resultData.wResult = world -> getResult();
            resultData.levelID = world -> getLevelID();
            resultData.rewardPlant = world -> getRewardPlant();

            if(resultData.wResult == WorldResult :: WON) {
                applyWinProgress(resultData);
            }

            requestTransition(
                ScreenAction :: PUSH,
                ScreenID :: GAME_RESULT,
                resultData
            );
        }
    }
}

void GameplayScreen :: applyWinProgress(const ScreenData& resultData) {
    if(!userProfileManager) return;

    UserProfile* profile = userProfileManager -> getActiveProfile();
    if(!profile) return;

    if(resultData.rewardPlant != PLANT_COUNT) {
        profile -> unlockPlant(resultData.rewardPlant);
    }

    LevelID nextLevel = resultData.levelID;
    nextLevel.stage++;

    auto levelExists = [](const LevelID& level) {
        const std :: filesystem :: path path =
            std :: filesystem :: path(PROJECT_DIR) /
            "assets/data/levels" /
            level.getFileName();
        return std :: filesystem :: exists(path);
    };

    if(!levelExists(nextLevel)) {
        LevelID nextWorld{resultData.levelID.world + 1, 1};
        if(levelExists(nextWorld)) nextLevel = nextWorld;
        else nextLevel = resultData.levelID;
    }

    const LevelID& highestLevel = profile -> getHighestUnlockedLevel();
    const bool advancesProgress =
        nextLevel.world > highestLevel.world ||
        (nextLevel.world == highestLevel.world &&
         nextLevel.stage > highestLevel.stage);

    if(advancesProgress) {
        profile -> setHighestUnlockedLevel(nextLevel);
    }
    userProfileManager -> saveProfiles();
}

void GameplayScreen :: draw() {
    if (world) {
        world -> draw();
    }

    if (world && world -> isChoosingPlants()) {
        choosePlants.draw();
    }

    if (world and (world -> isChoosingPlants() || world -> isReady())) {
        world -> drawPlacementPreview(seedBank.selectedPlantId());
        seedBank.draw();
    }

    if (world && world -> isReady()) {
        drawSunHUD();
        waveHUD.draw(
            world -> getWaveProgress(),
            world -> getCurrentWave(),
            world -> getTotalWaves(),
            world -> isWaveFinished(),
            screenWidth,
            screenHeight
        );
    }

    drawPauseButton();
}

Rectangle GameplayScreen::getPauseButtonBounds() const {
    return {
        static_cast<float>(screenWidth) - 58.0f,
        14.0f, 44.0f, 44.0f
    };
}

void GameplayScreen::drawPauseButton() const {
    const Rectangle bounds = getPauseButtonBounds();
    const bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);

    DrawRectangleRounded(
        bounds, 0.25f, 8,
        hovered ? Color{88, 93, 120, 235} : Color{45, 48, 65, 210}
    );
    DrawRectangleRoundedLinesEx(
        bounds, 0.25f, 8, 2.0f,
        hovered ? Color{255, 225, 120, 255} : Color{205, 210, 225, 255}
    );

    const float barWidth = 7.0f;
    const float barHeight = 22.0f;
    const float barY = bounds.y + (bounds.height - barHeight) * 0.5f;
    DrawRectangleRec(
        {bounds.x + 12.0f, barY, barWidth, barHeight},
        WHITE
    );
    DrawRectangleRec(
        {bounds.x + 25.0f, barY, barWidth, barHeight},
        WHITE
    );
}

void GameplayScreen :: drawSunHUD() const {
    if (!world || !textManager) return;

    // Panel positioned just below the seed bank on the left
    const Rectangle panel = { 2.0f, 80.0f, 82.0f, 34.0f };
    const float cornerRadius = 0.35f;
    const int segments = 8;

    // // Dark translucent background
    // DrawRectangleRounded(panel, cornerRadius, segments, Color{20, 12, 0, 210});
    // // Warm gold border
    // DrawRectangleRoundedLinesEx(panel, cornerRadius, segments, 2.0f, Color{220, 170, 0, 255});

    // Sun icon — a small filled circle in warm yellow
    DrawCircle((int)(panel.x + 14.0f), (int)(panel.y + panel.height * 0.5f), 10.0f, Color{255, 210, 0, 255});
    DrawCircleLines((int)(panel.x + 14.0f), (int)(panel.y + panel.height * 0.5f), 10.0f, Color{220, 160, 0, 255});

    // Sun count text, right portion of the panel
    const std::string sunText = std::to_string(world->getSunAmount());
    const Rectangle textRect = { panel.x + 26.0f, panel.y, panel.width - 28.0f, panel.height };
    textManager->drawCenteredText("Luckiest_Guy", sunText.c_str(), textRect, 20.0f, 1.0f, BLACK);
}

void GameplayScreen :: handleInput(const RawInputEvent& inputEvent) {
    if (!world) {
        return;
    }

    if (inputEvent.inputType == RawInputEvent::InputType::LEFT_MOUSE_CLICKED) {
        if (CheckCollisionPointRec(inputEvent.position, getPauseButtonBounds())) {
            requestTransition(ScreenAction::PUSH, ScreenID::PAUSE_MENU);
            return;
        }

        if (world -> isChoosingPlants()) {
            if (choosePlants.handleMouseClick(inputEvent.position) && choosePlants.isDone()) {
                seedBank.setSlots(choosePlants.choosePlants());
                if (world) seedBank.setSunCosts(world->getAllSunCosts());
                world -> finishChoosingPlants();
            }
            return;
        }

        if (!world -> isReady()) {
            return;
        }

        if (world -> handleParticleClick(inputEvent.position)) {
            return;
        }

        if (seedBank.handleMouseClick(inputEvent.position)) {
            return;
        }

        int selectedPlantId = seedBank.selectedPlantId();
        if (selectedPlantId >= 0) {
            world -> tryPlacePlant(inputEvent.position, (PlantType)selectedPlantId);
        }
    }
}
