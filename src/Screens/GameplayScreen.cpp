#include <Screens/GameplayScreen.hpp>

GameplayScreen :: GameplayScreen(int screenWidth, int screenHeight) {
    assetManager = std::make_unique<AssetManager>();
    world = std::make_unique<World>(screenWidth, screenHeight, assetManager.get());

    seedBank.setTexturePackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    seedBank.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
}

void GameplayScreen :: update(float dt) {
    if (world) {
        world -> update(dt);
    }
}

void GameplayScreen :: draw() {
    if (world) {
        world -> draw();
        world -> drawPlacementPreview(seedBank.selectedPlantId());
    }

    if (world && world -> isReady()) {
        seedBank.draw();
    }
}

void GameplayScreen :: handleInput(const RawInputEvent& inputEvent) {
    if (!world) {
        return;
    }

    if (inputEvent.inputType == RawInputEvent::InputType::LEFT_MOUSE_CLICKED) {
        if (!world -> isReady()) {
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
