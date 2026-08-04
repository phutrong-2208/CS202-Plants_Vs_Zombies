#include <Screens/GameplayScreen.hpp>

GameplayScreen :: GameplayScreen(int screenWidth, int screenHeight, AssetManager* manager) {
    setAssetManager(manager);
    world = std::make_unique<World>(screenWidth, screenHeight, assetManager);
    textManager = assetManager->getTextManager();

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
}

void GameplayScreen :: update(float dt) {
    if (world) {
        world -> update(dt);
    }
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
    }
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
