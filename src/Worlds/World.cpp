#include <Worlds/World.hpp>

///////////////////////////////
///     IGAMEPLAYMEDIATOR   ///
///////////////////////////////
void World::addProjectile(ProjectileType projType, Vector2 position, float damage) {
    auto bullet = projectileFactory.createProjectile(projType, position, damage);
    projectileManager.addProjectile(std::move(bullet));
}

World :: World(int screenWidth, int screenHeight, AssetManager* assetManager)
{
    map = std::make_unique <DayMap> ();
    if (assetManager == nullptr) {
        TraceLog(LOG_ERROR, "Asset Manager was not found");
        return;
    }

    TextureManager* textureManager = assetManager -> getTextureManager();

    plantFactory.setTextureManager(textureManager);
    plantFactory.setAnimationManager(assetManager -> getAnimationManager());
    plantFactory.loadPlantMechanics();

    zombieFactory.setTextureManager(textureManager);
    zombieFactory.setAnimationManager(assetManager -> getAnimationManager());
    zombieFactory.loadZombieMechanics();

    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0) zombieManager.addZombie(zombieFactory.createZombie(
            NORMAL_ZOMBIE, Rectangle {grid.getCellRect(i, 8).x, grid.getCellRect(i, 8).y - 40.0f, 50.0f, 100.0f}
        )); 

        if (i % 2 == 1) zombieManager.addZombie(zombieFactory.createZombie(
            DANCER_ZOMBIE, Rectangle {grid.getCellRect(i, 8).x, grid.getCellRect(i, 8).y - 40.0f, 50.0f, 100.0f}
        ));
    }

    grid.setMediator(this);
     

    // if (textureManager) {
    //     projectileTexturePackage = textureManager -> getPackage("Projectile");
    // }

    // if (!projectileTexturePackage) {
    //     TraceLog(LOG_WARNING, "Projectile texture package was not found");
    // }
}

void World :: update(float dt) {
    if (!map) return;

    map -> update(dt);
    if (isReady() == false) return;

    grid.updateTime(dt);
    projectileManager.update(dt);
    zombieManager.update(dt);


    grid.sendPlantAttacks();
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
