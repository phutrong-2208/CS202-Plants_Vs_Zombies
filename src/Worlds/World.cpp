#include <Worlds/World.hpp>

World :: World(int screenWidth, int screenHeight, AssetManager* assetManager)
{
    map = std::make_unique <DayMap> ();
    if (assetManager) {
        TextureManager* textureManager = assetManager -> getTextureManager();

        plantFactory.setTextureManager(textureManager);
        plantFactory.setAnimationManager(assetManager -> getAnimationManager());
        plantFactory.loadPlantMechanics();

        zombieFactory.setTextureManager(textureManager);
        zombieFactory.setAnimationManager(assetManager -> getAnimationManager());
        zombieFactory.loadZombieMechanics();

        zombieManager.addZombie(zombieFactory.createZombie(
            NORMAL_ZOMBIE, Rectangle{650.0f, 250.0f, 50.0f, 100.0f}
        )); 

        if (textureManager) {
            projectileTexturePackage = textureManager -> getPackage("Projectile");
        }

        if (!projectileTexturePackage) {
            TraceLog(LOG_WARNING, "Projectile texture package was not found");
        }
    }
}

void World :: update(float dt) {
    if (!map) return;

    map -> update(dt);
    if (isReady() == false) return;

    grid.updateTime(dt, projectileManager, zombieManager);
    projectileManager.update(dt);
    zombieManager.update(dt);
}

void World :: draw() {
    if (!map) return;

    map -> drawBackground();
    if (isReady() == false) return;

    grid.draw();
    projectileManager.simulate();
    zombieManager.draw();
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
