#include <Screens/GameplayScreen.hpp>
#include <Core/UserProfileManager.hpp>

#include <filesystem>

GameplayScreen :: GameplayScreen(int screenWidth, int screenHeight, AssetManager* manager, LevelID levelID, UserProfile* user, GameMode gm) : gameMode(gm) {
    setAssetManager(manager);
    world = std :: make_unique<World>(
        screenWidth, screenHeight, assetManager, levelID
    );
    textManager = assetManager -> getTextManager();
    this -> screenWidth  = screenWidth;
    this -> screenHeight = screenHeight;

    waveHUD.setTexturePackage(assetManager->getTextureManager()->getPackage("ZombiePhase"));

    seedBank.setTexturePackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    seedBank.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
    seedBank.setTextManager(assetManager -> getTextManager());
    seedBank.setSeedRechargeTimes(world -> getAllSeedRecharges());

    choosePlants.setChooserPackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    choosePlants.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
    choosePlants.setTextManager(assetManager -> getTextManager());
    choosePlants.setSunCosts(world -> getAllSunCosts());
    choosePlants.setAvailablePlants(getAllPlantTypes());
    choosePlants.setMaxSlots(6);
    choosePlants.setSeedBank(&seedBank);
    choosePlants.setUnlockedPlants(user -> getUnlockedPlants());

    shovelPackage = assetManager -> getTextureManager() -> getPackage("Shovel");

    if (gameMode == GameMode::SURVIVAL_ENDLESS) {
        endlessController = std :: make_unique<EndlessController>();
        endlessController -> attachToWaveManager(&world->getWaveManager());
        endlessController -> launch();
    }
}

void GameplayScreen :: update(float dt) {
    if (world) {
        if (endlessController) {
            
            world -> update(dt);
            if(world -> isReady()) seedBank.update(dt);

            const auto currentEndlessPhase = endlessController -> getPhase();
            if (currentEndlessPhase == EndlessController :: Phase :: WAVE_RUNNING
                && prevEndlessPhase != EndlessController :: Phase :: WAVE_RUNNING) {
                world -> setResult(WorldResult :: RUNNING);
            }
            prevEndlessPhase = currentEndlessPhase;

            endlessController -> update(
                dt,
                world -> isWaveFinished(),
                world -> getZombieCount() == 0,
                world -> getResult() == WorldResult :: LOST
            );
            

            if(!resultRequested &&
               endlessController -> getPhase() == EndlessController :: Phase :: GAME_OVER) {
                resultRequested = true;

                ScreenData resultData;
                resultData.wResult = WorldResult :: LOST;
                resultData.gameMode = GameMode :: SURVIVAL_ENDLESS;
                resultData.survivalScore = endlessController -> getScore();
                resultData.flagsCleared = endlessController -> getFlagsCleared();
                saveEndlessRecord(resultData);

                requestTransition(
                    ScreenAction :: PUSH,
                    ScreenID :: GAME_RESULT,
                    resultData
                );
            }
        }
        else {
            world -> update(dt);
            if(world -> isReady()) seedBank.update(dt);

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
}

void GameplayScreen :: saveEndlessRecord(const ScreenData& resultData) {
    if(!userProfileManager) return;

    UserProfile* profile = userProfileManager -> getActiveProfile();
    if(!profile) return;

    if(resultData.survivalScore > profile -> getHighestSurvivalScore()) {
        profile -> setHighestSurvivalScore(resultData.survivalScore);
        userProfileManager -> saveProfiles();
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
        world -> drawPlacementPreview(seedBank.selectedPlantId(), isShovelSelected);
        seedBank.draw();
    }

    if (world && world -> isReady()) {
        drawSunHUD();
        drawShovel();
        if (endlessController) {
            int highest = 0;
            if (userProfileManager && userProfileManager -> getActiveProfile()) {
                highest = userProfileManager -> getActiveProfile() -> getHighestSurvivalScore();
            }
            endlessHUD.draw(*endlessController, highest, textManager, screenWidth, screenHeight);
        }
        else {
            waveHUD.draw(
                world -> getWaveProgress(),
                world -> getCurrentWave(),
                world -> getTotalWaves(),
                world -> isWaveFinished(),
                screenWidth,
                screenHeight
            );
        }
    }

    drawPauseButton();
}

Rectangle GameplayScreen :: getShovelBounds() const {
    return {
        10.0f,
        static_cast<float>(screenHeight) - 80.0f,
        70.0f,
        70.0f
    };
}

void GameplayScreen :: drawShovel() const {
    if (!shovelPackage) return;
    const Rectangle bounds = getShovelBounds();
    Texture2D* bankTex = shovelPackage -> GetTexture("SHOVELBANK");
    Texture2D* shovelTex = shovelPackage -> GetTexture("SHOVEL");

    if (bankTex) {
        DrawTexturePro(*bankTex, {0, 0, (float)bankTex -> width, (float)bankTex -> height},
                       bounds, {0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangleRounded(bounds, 0.2f, 4, Color{70, 50, 30, 220});
        DrawRectangleRoundedLinesEx(bounds, 0.2f, 4, 2.0f, Color{220, 190, 120, 255});
    }

    if (!isShovelSelected && shovelTex) {
        Rectangle iconDst = { bounds.x + 8.0f, bounds.y + 8.0f, bounds.width - 16.0f, bounds.height - 16.0f };
        DrawTexturePro(*shovelTex, {0, 0, (float)shovelTex -> width, (float)shovelTex -> height},
                       iconDst, {0, 0}, 0.0f, WHITE);
    } else if (isShovelSelected && shovelTex) {
        Vector2 mouse = GetMousePosition();
        Rectangle mouseDst = { mouse.x - 12.0f, mouse.y - 48.0f, 58.0f, 58.0f };
        DrawTexturePro(*shovelTex, {0, 0, (float)shovelTex -> width, (float)shovelTex -> height},
                       mouseDst, {0, 0}, 0.0f, WHITE);
    }
}

Rectangle GameplayScreen :: getPauseButtonBounds() const {
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

    if (inputEvent.inputType == RawInputEvent::InputType::RIGHT_MOUSE_CLICKED) {
        isShovelSelected = false;
        seedBank.clearSelection();
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

        // Shovel Bank click
        if (CheckCollisionPointRec(inputEvent.position, getShovelBounds())) {
            isShovelSelected = !isShovelSelected;
            if (isShovelSelected) seedBank.clearSelection();
            return;
        }

        if (seedBank.handleMouseClick(inputEvent.position)) {
            isShovelSelected = false;
            return;
        }

        // Shoveling a plant on the lawn
        if (isShovelSelected) {
            int r, c;
            std::tie(r, c) = world->getGrid().getCellID(inputEvent.position);
            if (r >= 0 && c >= 0) {
                world->removePlant(r, c);
                isShovelSelected = false;
            } else {
                isShovelSelected = false;
            }
            return;
        }

        int selectedPlantId = seedBank.selectedPlantId();
        if (selectedPlantId >= 0) {
            PlantType selectedPlant = static_cast<PlantType>(selectedPlantId);
            if(world -> tryPlacePlant(inputEvent.position, selectedPlant)) seedBank.startCooldown(selectedPlant);
        }
    }
}
