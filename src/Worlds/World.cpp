#include <Worlds/World.hpp>

World :: World(int screenWidth, int screenHeight, AssetManager* assetManager)
{
    map = std::make_unique <DayMap> ();
    if (assetManager) {
        plantFactory.setTextureManager(assetManager -> getTextureManager());
        plantFactory.setAnimationManager(assetManager -> getAnimationManager());
        plantFactory.loadPlantMechanics();
    }
}

void World :: update(float dt) {
    if (!map) return;

    map -> update(dt);
    if (isReady() == false) return;

    grid.updateTime(dt);
}

void World :: draw() {
    if (!map) return;

    map -> drawBackground();
    if (isReady() == false) return;

    grid.draw();
}

void World :: drawPlacementPreview(int selectedPlantId) const {
    if (!map || isReady() == false || selectedPlantId < 0) return;

    Vector2 mouse = GetMousePosition();
    int hovR, hovC; std :: tie(hovR, hovC) = grid.getCellID(mouse);

    if (hovR != -1 && hovC != -1) {
        Rectangle rect = grid.getCellRect(hovR, hovC);
        DrawRectangleLinesEx(rect, 3, LIME);
    }
}

bool World :: tryPlacePlant(Vector2 position, PlantType plantType) {
    if (!map || isReady() == false) return false;

    int r, c; std :: tie(r, c) = grid.getCellID(position);
    if (r < 0 || c < 0 || grid.getPlant(r, c)) return false;
    return grid.placePlant(r, c, plantFactory.createPlant(plantType));
}

bool World :: isReady() const {
    return map && map -> isReady();
}

bool World :: isChoosingPlants() const {
    return map && map -> isChoosingPlants();
}

void World :: finishChoosingPlants() {
    if (map) {
        map -> finishChoosingPlants();
    }
}
