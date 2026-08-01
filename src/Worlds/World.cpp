#include <Worlds/World.hpp>

///////////////////////////////
///     IGAMEPLAYMEDIATOR   ///
///////////////////////////////

void World::addProjectile(PlantType plantType, Vector2 position, float damage) {
    if (projectileConvert.find(plantType) == projectileConvert.end()) return;

    auto bullet = projectileFactory.createProjectile(projectileConvert.at(plantType), position, damage);
    projectileManager.addProjectile(std::move(bullet));
}
bool World::hasTarget(PlantType plantType, Vector2 spawnPos, Rectangle bounds) {
    if (projectileConvert.find(plantType) == projectileConvert.end()) return false;

    float range = 0.0f;
    auto projData = projectileFactory.getProjectileData(projectileConvert.at(plantType));
    if (projData) range = projData->getRange();

    Rectangle sensor = {spawnPos.x, bounds.y, range, bounds.height};
    return zombieManager.hasZombieInArea(sensor);
}

bool World::touchTarget(Projectile* projectile) {
    if (projectile == nullptr) return false;
    Rectangle hitbox = projectile -> getHitbox();

    Zombie* zombie = zombieManager.getShotFirst(hitbox);

    if (zombie != nullptr) {
        zombie -> receiveDamage(projectile -> getDamage());
        return true;
    }

    return false;
}

bool World::hasPlantInArea(Rectangle area) const {
    return grid.hasPlantInArea(area);
}

bool World::damagePlantInArea(Rectangle area, float damage) {
    return grid.damagePlantInArea(area, damage);
}

World::World(int screenWidth, int screenHeight, AssetManager *assetManager) {
    map = std::make_unique<DayMap>();
    if (assetManager == nullptr) {
        TraceLog(LOG_ERROR, "Asset Manager was not found");
        return;
    }

    TextureManager *textureManager = assetManager->getTextureManager();

    plantFactory.setTextureManager(textureManager);
    plantFactory.setAnimationManager(assetManager->getAnimationManager());
    plantFactory.loadPlantMechanics();

    zombieFactory.setTextureManager(textureManager);
    zombieFactory.setAnimationManager(assetManager->getAnimationManager());
    zombieFactory.loadZombieMechanics();

    projectileFactory.setProjectileTexturePackage(textureManager -> getPackage("Projectile"));
    projectileFactory.loadProjectileMechanics();

    // Temporary ParticleManager smoke test. Remove after Sun spawning is wired.
    TexturePackage* particlePackage = textureManager -> getPackage("Particles");
    ReanimParser* sunAnimationData =
        assetManager -> getAnimationManager() -> getAnimationData("SunAnim");
    if(particlePackage && sunAnimationData){
        ReanimInstance sunAnimation;
        sunAnimation.setTexturePackage(particlePackage);
        sunAnimation.setAnimation(sunAnimationData);
        sunAnimation.setTextureScalar(1.0f);

        particleManager.addParticle(
            std :: make_unique<Sun>(
                std :: move(sunAnimation),
                Vector2{screenWidth * 0.5f, 100.0f},
                300.0f,
                25
            )
        );
    }

    for (int i = 0; i < 5; ++i) {
        if (i % 2 == 0)
        zombieManager.addZombie(zombieFactory.createZombie(
            NORMAL_ZOMBIE,
            Rectangle{grid.getCellRect(i, 8).x, grid.getCellRect(i, 8).y - 40.0f,
                        50.0f, 100.0f}));

        if (i % 2 == 1)
        zombieManager.addZombie(zombieFactory.createZombie(
            DANCER_ZOMBIE,
            Rectangle{grid.getCellRect(i, 8).x, grid.getCellRect(i, 8).y - 40.0f,
                        50.0f, 100.0f}));
    }

    grid.setMediator(this);
    projectileManager.setMediator(this);
    zombieManager.setMediator(this);
}

void World :: update(float dt) {
    if (!map)
        return;

    map->update(dt);
    if (isReady() == false)
        return;

    grid.updateTime(dt);
    projectileManager.update(dt);
    zombieManager.update(dt);
    particleManager.update(dt);

    grid.sendPlantAttacks();
    projectileManager.toggleProjectiles();
}

void World ::draw() {
    if (!map)
        return;

    map -> drawBackground();
    if (isReady() == false)
        return;

    grid.draw();
    projectileManager.simulate();
    zombieManager.draw();
    particleManager.draw();
}

void World :: drawPlacementPreview(int selectedPlantId) const {
    if (!map || isReady() == false || selectedPlantId < 0)
        return;

    Vector2 mouse = GetMousePosition();
    int hovR, hovC;
    std :: tie(hovR, hovC) = grid.getCellID(mouse);

    if (hovR != -1 && hovC != -1) {
        Rectangle rect = grid.getCellRect(hovR, hovC);
        DrawRectangleLinesEx(rect, 3, LIME);
    }
}

bool World :: tryPlacePlant(Vector2 position, PlantType plantType) {
    if (!map || isReady() == false)
        return false;

    int r, c;
    std :: tie(r, c) = grid.getCellID(position);
    if (r < 0 || c < 0 || grid.getPlant(r, c))
        return false;
    return grid.placePlant(r, c, plantFactory.createPlant(plantType));
}

bool World :: handleParticleClick(Vector2 position) {
    const int collectedValue = particleManager.handleClick(position);
    if(collectedValue <= 0) return false;

    sunAmount += collectedValue;
    return true;
}

int World :: getSunAmount() const {
    return sunAmount;
}

bool World :: isReady() const { return map && map->isReady(); }

bool World :: isChoosingPlants() const { return map && map->isChoosingPlants(); }

void World :: finishChoosingPlants() {
    if (map) {
        map -> finishChoosingPlants();
    }
}
