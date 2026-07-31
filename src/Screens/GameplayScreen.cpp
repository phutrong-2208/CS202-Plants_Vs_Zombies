#include <Screens/GameplayScreen.hpp>

GameplayScreen :: GameplayScreen(int screenWidth, int screenHeight, AssetManager* manager) {
    setAssetManager(manager);
    world = std::make_unique<World>(screenWidth, screenHeight, assetManager);

    seedBank.setTexturePackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    seedBank.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));

    choosePlants.setChooserPackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
    choosePlants.setPacketPackage(assetManager -> getTextureManager() -> getPackage("PlantSeedPackets"));
    choosePlants.setTextManager(assetManager -> getTextManager());
    choosePlants.setAvailablePlants({PEASHOOTER, SUNFLOWER, REPEATER, SNOWPEA, WALLNUT, CACTUS});
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
}

void GameplayScreen :: handleInput(const RawInputEvent& inputEvent) {
    if (!world) {
        return;
    }

    if (inputEvent.inputType == RawInputEvent::InputType::LEFT_MOUSE_CLICKED) {
        if (world -> isChoosingPlants()) {
            if (choosePlants.handleMouseClick(inputEvent.position) && choosePlants.isDone()) {
                seedBank.setSlots(choosePlants.choosePlants());
                world -> finishChoosingPlants();
            }
            return;
        }

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
