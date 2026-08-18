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

    aiPvZMode = gameMode == GameMode :: AI_PVZ;
    if(aiPvZMode) {
        world -> setPlayerControlsZombies(true);
        plantAI.setWorld(world.get());
        conveyorBelt.setTexturePackage(assetManager -> getTextureManager() -> getPackage("ConveyorBelt"));
        conveyorBelt.setPacketPackage(assetManager -> getTextureManager() -> getPackage("ZombiePacket"));
        conveyorBelt.setZombiePool(getAllZombiePacketTypes());
        conveyorBelt.setSpawnIntervalRange(12.0f, 20.0f);
        conveyorBelt.start(0);
    }

    choosePlants.setChooserPackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    choosePlants.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
    choosePlants.setTextManager(assetManager -> getTextManager());
    choosePlants.setSunCosts(world -> getAllSunCosts());
    choosePlants.setAvailablePlants(getAllPlantTypes());
    choosePlants.setMaxSlots(10);
    choosePlants.setSeedBank(&seedBank);
    if(user) choosePlants.setUnlockedPlants(user -> getUnlockedPlants());

    shovelPackage = assetManager -> getTextureManager() -> getPackage("Shovel");

    if (gameMode == GameMode::SURVIVAL_ENDLESS) {
        endlessController = std :: make_unique<EndlessController>();
        endlessController -> attachToWaveManager(&world->getWaveManager());
        endlessController -> launch();
    }

    if (assetManager && assetManager->getMusicManager()) {
        assetManager->getMusicManager()->play("ZOMBIESONYOURLAWN", true);
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
            if(aiPvZMode && world -> isChoosingPlants()) world -> finishChoosingPlants();
            if(world -> isReady()) {
                if (waveAnnouncementTimer > 0.0f) {
                    waveAnnouncementTimer -= dt;
                }

                int curWave = world -> getCurrentWave();
                if (curWave != lastAnnouncedWave && curWave >= 0) {
                    lastAnnouncedWave = curWave;
                    if (world -> isCurrentWaveFinal()) {
                        waveAnnouncementText = "THE FINAL WAVE ARRIVED";
                        waveAnnouncementTimer = 3.5f;
                        if (assetManager && assetManager -> getSoundManager()) {
                            assetManager -> getSoundManager() -> play("FINALWAVE", 1.0f);
                        }
                    } else if (world -> isCurrentWaveHuge()) {
                        waveAnnouncementText = "A BIGGER WAVE IS APPROACHING";
                        waveAnnouncementTimer = 3.5f;
                        if (assetManager && assetManager -> getSoundManager()) {
                            assetManager -> getSoundManager() -> play("HUGEWAVE", 1.0f);
                            assetManager -> getSoundManager() -> play("AWOOGA", 0.9f);
                        }
                    }
                }

                if(aiPvZMode) {
                    plantAI.update(dt);
                    conveyorBelt.update(dt);
                }
                else seedBank.update(dt);
            }

            if(!resultRequested && world -> getResult() != WorldResult :: RUNNING) {
                resultRequested = true;

                if (assetManager && assetManager->getMusicManager()) {
                    assetManager->getMusicManager()->stop();
                }

                ScreenData resultData;
                resultData.wResult = world -> getResult();
                resultData.levelID = world -> getLevelID();
                resultData.gameMode = gameMode;
                resultData.rewardPlant = aiPvZMode ? PLANT_COUNT : world -> getRewardPlant();

                if(resultData.wResult == WorldResult :: WON && !aiPvZMode) {
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
    if (world && aiPvZMode) {
        world -> drawBeforeZombies();
        if(world -> isChoosingPlants() || world -> isReady()) conveyorBelt.draw();
        world -> drawZombiesAndParticles();
    }
    else if (world) {
        world -> draw();
    }

    if (world && world -> isChoosingPlants() && !aiPvZMode) {
        choosePlants.draw();
    }

    if (world and (world -> isChoosingPlants() || world -> isReady())) {
        if(aiPvZMode) {
            world -> drawZombiePlacementPreview(
                conveyorBelt.selectedZombieType() != ZOMBIE_COUNT
            );
        }
        else {
            world -> drawPlacementPreview(seedBank.selectedPlantId(), isShovelSelected);
            seedBank.draw();
        }
    }

    if (world && world -> isReady()) {
        if(!aiPvZMode) {
            drawSunHUD();
            drawShovel();
        }
        else drawAIPvZCurrencyHUD();
        if (endlessController) {
            int highest = 0;
            if (userProfileManager && userProfileManager -> getActiveProfile()) {
                highest = userProfileManager -> getActiveProfile() -> getHighestSurvivalScore();
            }
            endlessHUD.draw(*endlessController, highest, textManager, screenWidth, screenHeight);
        }
        else if(!aiPvZMode) {
            waveHUD.draw(
                world -> getWaveProgress(),
                world -> getCurrentWave(),
                world -> getTotalWaves(),
                world -> isWaveFinished(),
                screenWidth,
                screenHeight
            );
        }

        drawWaveAnnouncement();
    }

    drawPauseButton();
}

void GameplayScreen :: drawWaveAnnouncement() const {
    if (waveAnnouncementTimer <= 0.0f || !textManager || waveAnnouncementText.empty()) return;

    float alpha = 1.0f;
    if (waveAnnouncementTimer < 0.6f) {
        alpha = waveAnnouncementTimer / 0.6f;
    } else if (waveAnnouncementTimer > 3.0f) {
        alpha = (3.5f - waveAnnouncementTimer) / 0.5f;
    }
    alpha = std::clamp(alpha, 0.0f, 1.0f);

    float pulse = 1.0f + 0.05f * sinf((3.5f - waveAnnouncementTimer) * 7.0f);
    float fontSize = 48.0f * pulse;

    Rectangle bannerRect = {
        0.0f,
        static_cast<float>(screenHeight) * 0.5f - 45.0f,
        static_cast<float>(screenWidth),
        90.0f
    };

    // Dark semi-transparent background bar for maximum readability
    DrawRectangle(
        0,
        static_cast<int>(bannerRect.y - 10.0f),
        screenWidth,
        static_cast<int>(bannerRect.height + 20.0f),
        ColorAlpha(BLACK, alpha * 0.5f)
    );

    // Deep black drop shadow
    Rectangle shadowRect = {
        bannerRect.x + 3.0f,
        bannerRect.y + 3.0f,
        bannerRect.width,
        bannerRect.height
    };
    textManager -> drawCenteredText(
        "Luckiest_Guy",
        waveAnnouncementText.c_str(),
        shadowRect,
        fontSize,
        1.5f,
        ColorAlpha(BLACK, alpha * 0.9f)
    );

    // Main red announcement text in Luckiest Guy font
    textManager -> drawCenteredText(
        "Luckiest_Guy",
        waveAnnouncementText.c_str(),
        bannerRect,
        fontSize,
        1.5f,
        ColorAlpha(RED, alpha)
    );
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

void GameplayScreen :: drawAIPvZCurrencyHUD() const {
    if(!world || !textManager) return;

    const Rectangle panel = {865.0f, 10.0f, 260.0f, 52.0f};
    DrawRectangleRounded(panel, 0.12f, 8, Color{35, 28, 42, 225});
    DrawRectangleRoundedLinesEx(panel, 0.12f, 8, 2.0f, Color{165, 115, 205, 255});

    DrawCircle(891, 35, 12.0f, Color{255, 210, 0, 255});
    DrawCircleLines(891, 35, 12.0f, Color{220, 160, 0, 255});
    textManager -> drawCenteredText(
        "LUCKIEST_GUY", TextFormat("AI SUN: %d", world -> getSunAmount()),
        {910.0f, 19.0f, 200.0f, 30.0f}, 19.0f, 0.7f, WHITE
    );
}

void GameplayScreen :: handleInput(const RawInputEvent& inputEvent) {
    if (!world) {
        return;
    }

    if (inputEvent.inputType == RawInputEvent::InputType::RIGHT_MOUSE_CLICKED) {
        isShovelSelected = false;
        seedBank.clearSelection();
        conveyorBelt.clearSelection();
        return;
    }

    if (inputEvent.inputType == RawInputEvent::InputType::LEFT_MOUSE_CLICKED) {
        if (CheckCollisionPointRec(inputEvent.position, getPauseButtonBounds())) {
            if (assetManager && assetManager->getSoundManager()) {
                assetManager->getSoundManager()->play("PAUSE");
            }
            requestTransition(ScreenAction::PUSH, ScreenID::PAUSE_MENU);
            return;
        }

        if (world -> isChoosingPlants()) {
            if(aiPvZMode) {
                world -> finishChoosingPlants();
                return;
            }
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

        if(aiPvZMode) {
            if(conveyorBelt.handleMouseClick(inputEvent.position)) return;

            const ZombieType selectedZombie = conveyorBelt.selectedZombieType();
            if(selectedZombie != ZOMBIE_COUNT &&
               world -> trySpawnPlayerZombie(inputEvent.position, selectedZombie)) {
                conveyorBelt.consumeSelectedPacket();
            }
            return;
        }

        if (world -> handleParticleClick(inputEvent.position)) {
            return;
        }

        // Shovel Bank click
        if (CheckCollisionPointRec(inputEvent.position, getShovelBounds())) {
            isShovelSelected = !isShovelSelected;
            if (assetManager && assetManager->getSoundManager()) {
                assetManager->getSoundManager()->play("SHOVEL");
            }
            if (isShovelSelected) {
                seedBank.clearSelection();
                conveyorBelt.clearSelection();
            }
            return;
        }

        if (seedBank.handleMouseClick(inputEvent.position)) {
            if (assetManager && assetManager->getSoundManager()) {
                assetManager->getSoundManager()->play("SEEDLIFT");
            }
            isShovelSelected = false;
            return;
        }

        // Shoveling a plant on the lawn
        if (isShovelSelected) {
            int r, c;
            std::tie(r, c) = world->getGrid().getCellID(inputEvent.position);
            if (r >= 0 && c >= 0) {
                world->removePlant(r, c);
                if (assetManager && assetManager->getSoundManager()) {
                    assetManager->getSoundManager()->play("SHOVEL");
                }
                isShovelSelected = false;
            } else {
                isShovelSelected = false;
            }
            return;
        }

        int selectedPlantId = seedBank.selectedPlantId();
        if (selectedPlantId >= 0) {
            PlantType selectedPlant = static_cast<PlantType>(selectedPlantId);
            if(world -> tryPlacePlant(inputEvent.position, selectedPlant)) {
                seedBank.startCooldown(selectedPlant);
            }
        }
    }
}
