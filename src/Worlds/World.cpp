#include <Worlds/World.hpp>

World :: World(int screenWidth, int screenHeight)
{
    map = std::make_unique <DayMap> ();
    assetManager = std::make_unique <AssetManager> ();
    plantFactory.setTextureManager(assetManager -> getTextureManager());
    plantFactory.setAnimationManager(assetManager -> getAnimationManager());
    seedBank.setTexturePackage(assetManager -> getTextureManager() -> getPackage("PlantChooser"));
}

void World :: update(float dt) {
    if (!map) return;

    map -> update(dt);
    if (map -> isReady() == false) return;

    grid.updateTime(dt);
}

void World :: draw() {
    if (!map) return;

    map -> drawBackground();
    if (map -> isReady() == false) return;

    grid.draw();

    Vector2 mouse = GetMousePosition();
    int hovR, hovC; std :: tie(hovR, hovC) = grid.getCellID(mouse);

    if (hovR != -1 && hovC != -1 && seedBank.selectedPlantId() >= 0) {
        Rectangle rect = grid.getCellRect(hovR, hovC);
        DrawRectangleLinesEx(rect, 3, LIME);
    }

    seedBank.draw();
}

void World :: onMouseClick(Vector2 position) {
    if (!map) return;

    if (map -> isReady() == false) return;
    if (seedBank.handleMouseClick(position)) return;

    int selectedPlantId = seedBank.selectedPlantId();
    if (selectedPlantId < 0) return;

    int r, c; std::tie(r, c) = grid.getCellID(position);
    if (r < 0 || c < 0) return;
    if (!grid.getPlant(r, c))
        grid.placePlant(r, c, plantFactory.createPlant((PlantType) selectedPlantId));
}
