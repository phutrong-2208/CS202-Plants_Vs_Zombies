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
        zombie -> receiveDamage(projectile -> getDamage(), this);
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

void World::addParticle(std::unique_ptr<Particle> particle) {
    particleManager.addParticle(std::move(particle));
}

void World::spawnSun(Vector2 position, float targetY, int value) {
    if (sunPackage && sunAnimationData) {
        ReanimInstance sunAnimation;
        sunAnimation.setTexturePackage(sunPackage);
        sunAnimation.setAnimation(sunAnimationData);
        sunAnimation.setTextureScalar(1.0f);

        particleManager.addParticle(
            std :: make_unique<Sun>(
                std :: move(sunAnimation), position, targetY, value
            )
        );
    }
}

bool World :: canAfford(PlantType type) const {
    return sunAmount >= plantFactory.getSunCost(type);
}

void World :: spendSun(int amount) {
    sunAmount = std::max(0, sunAmount - amount);
}

void World :: resetSkySunTimer() {
    if(!currentLevel || !currentLevel -> isSkySunEnabled()) {
        skySunTimer = 0.0f;
        return;
    }

    const float minInterval = std :: max(0.1f, currentLevel -> getSkySunIntervalMin());
    const float maxInterval = std :: max(minInterval, currentLevel -> getSkySunIntervalMax());
    const int minMs = static_cast<int>(minInterval * 1000.0f);
    const int maxMs = static_cast<int>(maxInterval * 1000.0f);
    skySunTimer = GetRandomValue(minMs, maxMs) / 1000.0f;
}

void World :: updateSkySun(float dt) {
    if(!currentLevel || !currentLevel -> isSkySunEnabled() || wResult != WorldResult :: RUNNING || activeLanes.empty()) return;
    skySunTimer -= dt;
    if(skySunTimer > 0.0f) return;
    spawnSkySun();
    resetSkySunTimer();
}

void World :: spawnSkySun() {
    if(!currentLevel || activeLanes.empty()) return;

    const int lane = activeLanes[GetRandomValue(0, static_cast<int>(activeLanes.size()) - 1)];
    const Rectangle firstCell = grid.getCellRect(lane, 0);
    const Rectangle lastCell = grid.getCellRect(lane, 8);
    const int minX = static_cast<int>(firstCell.x + 35.0f);
    const int maxX = static_cast<int>(lastCell.x + lastCell.width - 35.0f);
    const int minY = static_cast<int>(firstCell.y + 25.0f);
    const int maxY = static_cast<int>(firstCell.y + firstCell.height - 25.0f);
    const Vector2 position = {static_cast<float>(GetRandomValue(minX, maxX)), -45.0f};
    const float targetY = static_cast<float>(GetRandomValue(minY, maxY));
    spawnSun(position, targetY, currentLevel -> getSkySunValue());
}

World::World(int screenWidth, int screenHeight, AssetManager* assetManager, LevelID levelID) {
    if (assetManager == nullptr) {
        TraceLog(LOG_ERROR, "Asset Manager was not found");
        return;
    }

    currentLevel = std :: make_unique<Level>(levelID);
    sunAmount = currentLevel -> getStartingSun();

    int grassLaneCount = 0;
    const std :: vector<LaneType>& levelLanes = currentLevel -> getLanes();

    for(int lane = 0; lane < static_cast<int>(levelLanes.size()); ++lane) {
        if(levelLanes[lane] == LaneType :: GRASS) grassLaneCount++;
        if(levelLanes[lane] != LaneType :: INACTIVE) activeLanes.push_back(lane);
    }

    map = std::make_unique<DayMap>(
        assetManager,
        grassLaneCount,
        currentLevel -> usesSodRollIntro()
    );

    TextureManager *textureManager = assetManager->getTextureManager();

    plantFactory.setTextureManager(textureManager);
    plantFactory.setAnimationManager(assetManager->getAnimationManager());
    plantFactory.loadPlantMechanics();

    zombieFactory.setTextureManager(textureManager);
    zombieFactory.setAnimationManager(assetManager->getAnimationManager());
    zombieFactory.loadZombieMechanics();

    projectileFactory.setProjectileTexturePackage(textureManager -> getPackage("Projectile"));
    projectileFactory.loadProjectileMechanics();

    this->sunPackage = textureManager -> getPackage("Particles");
    this->sunAnimationData = assetManager -> getAnimationManager() -> getAnimationData("SunAnim");
    resetSkySunTimer();

    waveManager.loadWaves(
        currentLevel -> getWaves(),
        activeLanes,
        currentLevel -> getFirstWaveDelay(),
        currentLevel -> getBetweenWaveDelay()
    );

    grid.setMediator(this);
    projectileManager.setMediator(this);
    zombieManager.setMediator(this);
}

void World :: update(float dt) {
    if (!map)
        return;

    map -> update(dt);
    if (isReady() == false)
        return;

    updateSkySun(dt);
    grid.updateTime(dt);
    projectileManager.update(dt);
    zombieManager.update(dt);
    particleManager.update(dt);
    waveManager.update(dt, *this);

    grid.sendPlantActions();
    projectileManager.toggleProjectiles();
    updateWorldState();
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

void World :: drawPlacementPreview(int selectedPlantId) const { //need to improved
    if (!map || isReady() == false || selectedPlantId < 0)
        return;

    Vector2 mouse = GetMousePosition();
    int hovR, hovC;
    std :: tie(hovR, hovC) = grid.getCellID(mouse);

    if(currentLevel -> getLanes()[grid.getCellID(mouse).first] == LaneType :: INACTIVE){
        return;
    }

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

    if(currentLevel -> getLanes()[r] == LaneType :: INACTIVE){
        return false;
    }


    if (!canAfford(plantType))
        return false;

    bool placed = grid.placePlant(r, c, plantFactory.createPlant(plantType));
    if (placed) spendSun(plantFactory.getSunCost(plantType));
    return placed;
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

std::map<PlantType, int> World :: getAllSunCosts() const {
    return plantFactory.getAllSunCosts();
}

std::map<PlantType, float> World :: getAllSeedRecharges() const {
    return plantFactory.getAllSeedRecharges();
}

void World::spawnZombie(ZombieType type, int lane) {
    if (lane < 0 || lane > 4) return;
    Rectangle spawnRect = grid.getCellRect(lane, 8);
    zombieManager.addZombie(
        zombieFactory.createZombie(
            type,
            Rectangle{spawnRect.x + 100.0f, spawnRect.y - 40.0f, 50.0f, 100.0f}
        )
    );
}

float World :: getWaveProgress() const { return waveManager.getProgress(); }
int World :: getCurrentWave() const { return waveManager.getCurrentWave(); }
int World :: getTotalWaves() const { return waveManager.getTotalWaves(); }
bool World :: isWaveFinished() const { return waveManager.isFinished(); }

bool World :: isReady() const { return map && map->isReady(); }

bool World :: isChoosingPlants() const { return map && map->isChoosingPlants(); }

void World :: finishChoosingPlants() {
    if (map) {
        map -> finishChoosingPlants();
    }
}

void World :: setResult(WorldResult result) {
    wResult = result;
}

WorldResult World :: getResult() const {
    return wResult;
}

const LevelID& World :: getLevelID() const {
    static const LevelID defaultLevelID;
    return currentLevel ? currentLevel -> getID() : defaultLevelID;
}

PlantType World :: getRewardPlant() const {
    return currentLevel ? currentLevel -> getRewardPlant() : PLANT_COUNT;
}

void World :: updateWorldState() {
    if(wResult != WorldResult :: RUNNING) return;

    if(zombieManager.hasZombieReachedHouse(0.0f)){
        wResult = WorldResult :: LOST;
    }
    else if(waveManager.hasSpawnAll() and zombieManager.empty()){
        wResult = WorldResult :: WON;
    }
}


void World :: setUserProfileManager(UserProfileManager* user){
    userManager = user;
}
