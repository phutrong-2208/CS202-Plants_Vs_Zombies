#include <Worlds/World.hpp>
#include <Gameplay/Particle/ReanimParticle.hpp>

///////////////////////////////
///     IGAMEPLAYMEDIATOR   ///
///////////////////////////////

void World::addProjectile(PlantType pType, Vector2 spawnPos, float damage, bool reverse) {
    if (projectileConvert.find(pType) == projectileConvert.end()) return;

    ProjectileType projType = projectileConvert.at(pType);
    auto projectile = projectileFactory.createProjectile(projType, spawnPos, damage);
    if (projectile) {
        if (reverse) {
            Vector2 vel = projectile->getVelocity();
            vel.x = -vel.x;
            projectile->setVelocity(vel);
        }
        
        if (projectile->isLobbed()) {
            Zombie* targetZombie = nullptr;
            float minTargetX = spawnPos.x + projectile->getRange(); 
            for (auto& z : zombieManager.getZombies()) {
                if (!z->isDead() && abs(z->getHitbox().y - spawnPos.y) < 100.0f && z->getHitbox().x > spawnPos.x) {
                    if (z->getHitbox().x < minTargetX) {
                        minTargetX = z->getHitbox().x;
                        targetZombie = z.get();
                    }
                }
            }
            Vector2 targetPos = {minTargetX, spawnPos.y + 20.0f}; // ground approximation
            if (targetZombie) {
                targetPos.y = targetZombie->getHitbox().y + targetZombie->getHitbox().height - 20.0f; // Target their feet area
            }
            projectile->setTarget(targetPos);
        }

        if (projType == PROJECTILE_PUFF) {
            playSound("PUFF", 0.7f);
        } else if (projType == PROJECTILE_CORN) {
            playSound("KERNELPULT", 0.7f);
        } else if (projType == PROJECTILE_FIREPEA) {
            playSound("FIREPEA", 0.7f);
        } else {
            playSound(GetRandomValue(0, 1) ? "THROW" : "THROW2", 0.6f);
        }

        projectileManager.addProjectile(std::move(projectile));
    }
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
    Rectangle hitbox = projectile -> getCollisionHitbox();

    Zombie* zombie = zombieManager.getZombiePriority(hitbox);

    if (zombie != nullptr) {
        if (projectile->getChillDuration() > 0.0f) {
            zombie->chill(projectile->getChillDuration());
        }
        
        zombie -> receiveDamage(projectile -> getDamage(), this);
        playSound(GetRandomValue(0, 1) ? "SPLAT" : "SPLAT2", 0.6f);

        return true;
    }

    return false;
}

void World::explodeProjectile(Projectile* projectile) {
    if(!projectile || !projectile -> isLobbed()) return;

    Vector2 center = projectile -> getPosition();
    Vector2 splash = projectile -> getSplashArea();

    Rectangle splashArea = {
        center.x - splash.x * 0.5f,
        center.y - splash.y * 0.5f,
        splash.x,
        splash.y
    };

    if (projectile->getChillDuration() > 0) {
        freezeZombiesInArea(splashArea, projectile->getChillDuration());
    }
    damageZombiesInArea(splashArea, projectile -> getDamage());
}

void World::tryIgniteProjectile(Rectangle area) {
    for (auto& projectile : projectileManager.getProjectiles()) {
        if (projectile->isDespawned()) continue;
        if (projectile->getType() == PROJECTILE_PEA && CheckCollisionRecs(area, projectile->getHitbox())) {
            projectile->setType(PROJECTILE_FIREPEA);
            projectile->setDamage(projectile->getDamage() * 2.0f);
            ProjectileData* fireData = projectileFactory.getProjectileData(PROJECTILE_FIREPEA);
            projectile->setProjectileData(fireData);
            projectile->setTexture(projectileFactory.getProjectileTexture(PROJECTILE_FIREPEA));

            ReanimParser* firePeaAnim = assetManager ? assetManager->getAnimationManager()->getAnimationData("FirePeaAnim") : nullptr;
            TexturePackage* projPack = assetManager ? assetManager->getTextureManager()->getPackage("Projectile") : nullptr;
            if (firePeaAnim && projPack) {
                ReanimInstance fireInstance(1.0f, projPack, firePeaAnim);
                fireInstance.setLoopToggle(true);
                projectile->setReanimInstance(std::move(fireInstance));
            }
        }
    }
}

bool World::hasPlantInArea(Rectangle area) const {
    return grid.hasPlantInArea(area);
}

bool World::damagePlantInArea(Rectangle area, float damage, Zombie* attacker) {
    return grid.damagePlantInArea(area, damage, attacker);
}

void World::killPlantsInArea(Rectangle area) {
    grid.killPlantsInArea(area);
}

void World::changeZombieLane(Zombie* zombie) {
    if (!zombie) return;
    
    Rectangle hitbox = zombie->getHitbox();
    float yCenter = hitbox.y + hitbox.height * 0.5f;
    std::pair<int, int> cell = grid.getCellID({hitbox.x, yCenter});
    int currentRow = cell.first;
    if (currentRow == -1) return; // Not on grid
    
    // Simple logic: shift up or down based on current lane.
    int newRow = currentRow;
    if (currentRow == 0) newRow = 1;
    else if (currentRow == 4) newRow = 3;
    else newRow = currentRow + (GetRandomValue(0, 1) == 0 ? 1 : -1);
    
    Rectangle newCellRect = grid.getCellRect(newRow, 0); // Get y from the row
    float newYCenter = newCellRect.y + newCellRect.height * 0.5f;
    float yOffset = newYCenter - yCenter;
    
    hitbox.y += yOffset;
    zombie->setHitbox(hitbox);
}

void World::killZombiesOfType(ZombieType type) {
    for (auto& zombie : zombieManager.getZombies()) {
        if (!zombie->isDead() && zombie->getType() == type) {
            zombie->receiveDamage(zombie->getHealth(), this);
        }
    }
}

bool World::hasZombieInArea(Rectangle area) const {
    return zombieManager.hasZombieInArea(area);
}

void World::damageZombiesInArea(Rectangle area, float damage, bool isExplosion) {
    if (isExplosion) {
        playSound("EXPLOSION", 1.0f);
    }
    for (auto& zombie : zombieManager.getZombies()) {
        if (!zombie->isDead() && CheckCollisionRecs(zombie->getHitbox(), area)) {
            if (isExplosion) {
                ReanimInstance charredAnim = zombieFactory.createCharredReanim();
                zombie->triggerCharred(std::move(charredAnim));
            } else {
                zombie->receiveDamage(damage, this);
            }
        }
    }
}

void World::killZombiesInArea(Rectangle area, bool isExplosion) {
    damageZombiesInArea(area, 99999.0f, isExplosion);
}

Zombie* World::getZombiePriority(Rectangle area) {
    return zombieManager.getZombiePriority(area);
}

bool World::stripArmorInArea(Rectangle area) {
    Zombie* target = zombieManager.getZombieWithArmor(area);
    if (target) {
        target->setArmorHealth(0.0f);
        return true;
    }
    return false;
}

void World::freezeZombiesInArea(Rectangle area, float duration) {
    playSound("FROZEN", 1.0f);
    for (auto& zombie : zombieManager.getZombies()) {
        if (!zombie->isDead() && CheckCollisionRecs(zombie->getHitbox(), area)) {
            // Need to implement freeze in Zombie first!
            zombie->freeze(duration);
        }
    }
}

void World::spawnExplosionParticles(Vector2 position, PlantType type) {
    TexturePackage* pack = sunPackage;
    if (!pack) {
        TraceLog(LOG_ERROR, "spawnExplosionParticles: sunPackage is NULL");
        return;
    }

    if (type == CHERRYBOMB) {
        Texture2D* blastMark = pack->GetTexture("BLASTMARK");
        Texture2D* pow = pack->GetTexture("POW");
        
        if (blastMark) {
            auto mark = std::make_unique<Particle>(blastMark, Vector2{position.x, position.y + 20}, Vector2{0, 0}, Vector2{0, 0}, 4.0f, 1.0f);
            addParticle(std::move(mark));
        }
        if (pow) {
            auto p = std::make_unique<Particle>(pow, Vector2{position.x, position.y}, Vector2{0, 0}, Vector2{0, 0}, 1.0f, 1.0f);
            addParticle(std::move(p));
        }
    } else if (type == DOOMSHROOM) {
        Texture2D* blastMark = pack->GetTexture("BLASTMARK");
        Texture2D* bossExp = pack->GetTexture("BOSSEXPLOSION3");
        Texture2D* base = pack->GetTexture("DOOMSHROOM_EXPLOSION_BASE");
        Texture2D* stem = pack->GetTexture("DOOMSHROOM_EXPLOSION_STEM");
        Texture2D* top = pack->GetTexture("DOOMSHROOM_EXPLOSION_TOP");
        Texture2D* doom = pack->GetTexture("DOOM");

        if (blastMark) {
            auto mark = std::make_unique<Particle>(blastMark, Vector2{position.x, position.y + 40}, Vector2{0, 0}, Vector2{0, 0}, 10.0f, 1.5f);
            addParticle(std::move(mark));
        }
        if (bossExp) {
            auto exp = std::make_unique<Particle>(bossExp, Vector2{position.x, position.y - 40}, Vector2{0, 0}, Vector2{0, 0}, 1.2f, 1.8f);
            addParticle(std::move(exp));
        }
        if (base) {
            auto p = std::make_unique<Particle>(base, Vector2{position.x, position.y + 20}, Vector2{0, 0}, Vector2{0, 0}, 1.5f, 1.0f);
            addParticle(std::move(p));
        }
        if (stem) {
            auto p = std::make_unique<Particle>(stem, Vector2{position.x, position.y - 80}, Vector2{0, -50}, Vector2{0, 0}, 1.5f, 1.0f);
            addParticle(std::move(p));
        }
        if (top) {
            auto p = std::make_unique<Particle>(top, Vector2{position.x, position.y - 180}, Vector2{0, -80}, Vector2{0, 0}, 1.5f, 1.0f);
            addParticle(std::move(p));
        }
        if (doom) {
            auto p = std::make_unique<Particle>(doom, Vector2{position.x, position.y - 250}, Vector2{0, -100}, Vector2{0, 0}, 1.5f, 1.0f);
            addParticle(std::move(p));
        }
    } else if (type == POTATOMINE) {
        Texture2D* blastMark = pack->GetTexture("BLASTMARK");
        Texture2D* spudow = pack->GetTexture("EXPLOSIONSPUDOW");
        Texture2D* flash = pack->GetTexture("POTATOMINEFLASH");
        Texture2D* parts = pack->GetTexture("POTATOMINE_PARTICLES");

        if (blastMark) {
            auto mark = std::make_unique<Particle>(blastMark, Vector2{position.x, position.y + 10}, Vector2{0, 0}, Vector2{0, 0}, 3.0f, 0.6f);
            addParticle(std::move(mark));
        }
        if (flash) {
            auto p = std::make_unique<Particle>(flash, Vector2{position.x, position.y}, Vector2{0, 0}, Vector2{0, 0}, 0.5f, 1.0f);
            addParticle(std::move(p));
        }
        if (spudow) {
            auto p = std::make_unique<Particle>(spudow, Vector2{position.x, position.y - 20}, Vector2{0, 0}, Vector2{0, 0}, 1.0f, 1.0f);
            addParticle(std::move(p));
        }
        if (parts) {
            for (int i = 0; i < 5; i++) {
                auto p = std::make_unique<Particle>(parts, Vector2{position.x, position.y}, Vector2{(float)GetRandomValue(-150, 150), (float)GetRandomValue(-200, -50)}, Vector2{0, 300}, 1.0f, 1.0f);
                addParticle(std::move(p));
            }
        }
    } else if (type == SQUASH) {

    } else if (type == JALAPENO) {
        ReanimParser* fireAnim = assetManager ? assetManager->getAnimationManager()->getAnimationData("fireAnim") : nullptr;
        if (fireAnim && pack) {
            std::pair<int, int> cellId = grid.getCellID(position);
            int row = cellId.first;
            if (row < 0 || row >= 5) {
                float minDist = 9999.0f;
                row = 0;
                for (int r = 0; r < 5; ++r) {
                    Rectangle cr = grid.getCellRect(r, 0);
                    float dist = fabsf((cr.y + cr.height * 0.5f) - position.y);
                    if (dist < minDist) {
                        minDist = dist;
                        row = r;
                    }
                }
            }
            for (int col = 0; col < 9; ++col) {
                Rectangle cellRect = grid.getCellRect(row, col);
                float fireX = cellRect.x + cellRect.width * 0.5f;
                float fireY = cellRect.y + cellRect.height * 0.5f;
                const float scalar = 1.3f;
                ReanimInstance fireInstance(scalar, pack, fireAnim);
                // Offset fireBounds so internal track offsets (x=-45, y=30) center the flame in the cell
                Rectangle fireBounds = {
                    cellRect.x + cellRect.width * 0.5f + (45.0f * scalar),
                    cellRect.y + cellRect.height * 0.5f - (30.0f * scalar) - 10.0f,
                    cellRect.width,
                    cellRect.height
                };
                float delay = 0.0f; // All ignite across the whole line immediately!
                float duration = 0.65f + col * 0.08f; // Extinguish sequentially from left to right!
                auto fireP = std::make_unique<ReanimParticle>(std::move(fireInstance), Vector2{fireX, fireY}, fireBounds, duration, "anim_flame", delay, 1.0f, WHITE, true);
                addParticle(std::move(fireP));
            }
        }
    } else if (type == ICESHROOM) {

    } else if (type == TANGLEKELP) {
        Texture2D* pow = pack->GetTexture("EXPLOSIONCLOUD"); // Splash approximation
        if (pow) {
            auto p = std::make_unique<Particle>(pow, Vector2{position.x, position.y + 20}, Vector2{0, -50}, Vector2{0, 50}, 1.0f, 0.5f);
            addParticle(std::move(p));
        }
    }
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

World :: World(int screenWidth, int screenHeight, AssetManager* assetManager, LevelID levelID) {
    if (assetManager == nullptr) {
        TraceLog(LOG_ERROR, "Asset Manager was not found");
        return;
    }

    this->assetManager = assetManager;

    currentLevel = std :: make_unique<Level>(levelID);
    sunAmount = currentLevel -> getStartingSun();

    int grassLaneCount = 0;
    const std :: vector<LaneType>& levelLanes = currentLevel -> getLanes();

    for(int lane = 0; lane < static_cast<int>(levelLanes.size()); ++lane) {
        if(levelLanes[lane] == LaneType :: GRASS) grassLaneCount++;
        if(levelLanes[lane] != LaneType :: INACTIVE) activeLanes.push_back(lane);
    }

    map = std :: make_unique<DayMap>(
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
    zombieManager.getDeathHandler().initialize(sunPackage, this);
    this->assetManager = assetManager;

    initLawnMowers();
}

void World::initLawnMowers() {
    if (!assetManager) return;
    TexturePackage* lmPack = assetManager->getTextureManager()->getPackage("LawnMower");
    ReanimParser* lmAnim = assetManager->getAnimationManager()->getAnimationData("LawnMowerAnim");
    lawnMowers.clear();
    for (int r = 0; r < 5; ++r) {
        LawnMower lm;
        Rectangle cell0 = grid.getCellRect(r, 0);
        Vector2 pos = { cell0.x - 55.0f, cell0.y + 20.0f };
        lm.init(r, pos, lmPack, lmAnim);
        lawnMowers.push_back(std::move(lm));
    }
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
    for (auto& lm : lawnMowers) {
        lm.update(dt, this);
    }
    particleManager.update(dt);
    if(playerControlsZombies) sunAmount += particleManager.collectAll();
    if(!playerControlsZombies) waveManager.update(dt, *this);

    grid.sendPlantActions();
    projectileManager.toggleProjectiles();
    updateWorldState();
}

void World ::draw() {
    drawBeforeZombies();
    drawZombiesAndParticles();
}

void World :: drawBeforeZombies() {
    if (!map)
        return;

    map -> drawBackground();
    if (isReady() == false)
        return;

    for (const auto& lm : lawnMowers) {
        lm.draw();
    }
    grid.draw();
    projectileManager.simulate();
}

void World :: drawZombiesAndParticles() {
    if (!map || !isReady()) return;
    zombieManager.draw();
    particleManager.draw();
}

void World :: drawPlacementPreview(int selectedPlantId, bool isShovelActive) const {
    if (!map || isReady() == false || (selectedPlantId < 0 && !isShovelActive))
        return;

    Vector2 mouse = GetMousePosition();
    int hovR, hovC;
    std :: tie(hovR, hovC) = grid.getCellID(mouse);

    if (hovR < 0 || hovC < 0) return;

    if (currentLevel && currentLevel -> getLanes()[hovR] == LaneType :: INACTIVE) {
        return;
    }

    // 1. Draw white fade on the entire hovered row
    for (int c = 0; c < 9; ++c) {
        Rectangle rRect = grid.getCellRect(hovR, c);
        DrawRectangleRec(rRect, Color{255, 255, 255, 38});
    }

    // 2. Draw white fade on the entire hovered column
    for (int r = 0; r < 5; ++r) {
        Rectangle cRect = grid.getCellRect(r, hovC);
        DrawRectangleRec(cRect, Color{255, 255, 255, 38});
    }

    // 3. Highlight target intersection cell
    Rectangle cellRect = grid.getCellRect(hovR, hovC);
    DrawRectangleRec(cellRect, Color{255, 255, 255, 75});
    DrawRectangleLinesEx(cellRect, 2.0f, Color{255, 255, 255, 220});
}

void World :: drawZombiePlacementPreview(bool hasSelectedZombie) const {
    if (!map || !isReady() || !hasSelectedZombie) return;

    const Vector2 mouse = GetMousePosition();
    int row, column;
    std :: tie(row, column) = grid.getCellID(mouse);
    if (row < 0 || column < 0) return;
    if (currentLevel && (
        row >= static_cast<int>(currentLevel -> getLanes().size()) ||
        currentLevel -> getLanes()[row] == LaneType :: INACTIVE
    )) return;

    const Rectangle cell = grid.getCellRect(row, 8);
    DrawRectangleRec(cell, Color{175, 75, 255, 45});
    DrawRectangleLinesEx(cell, 2.0f, Color{220, 155, 255, 190});
}

void World::removePlant(int row, int col) {
    grid.removePlant(row, col);
}

bool World :: tryPlacePlant(Vector2 position, PlantType plantType, bool ignoreSunCost) {
    if (!map || isReady() == false)
        return false;

    int r, c;
    std :: tie(r, c) = grid.getCellID(position);
    if (r < 0 || c < 0)
        return false;

    return tryPlacePlantAtCell(r, c, plantType, ignoreSunCost);
}

bool World :: tryPlacePlantAtCell(int row, int col, PlantType plantType, bool ignoreSunCost) {
    if (!map || !isReady() || !currentLevel) return false;
    if (row < 0 || row >= static_cast<int>(currentLevel -> getLanes().size())) return false;
    if (col < 0 || col >= 9 || currentLevel -> getLanes()[row] == LaneType :: INACTIVE) return false;
    if (plantType < 0 || plantType >= PLANT_COUNT) return false;
    if (!ignoreSunCost && !canAfford(plantType)) return false;

    const bool placed = grid.placePlant(row, col, plantFactory.createPlant(plantType));
    if (placed) {
        if (!ignoreSunCost) spendSun(plantFactory.getSunCost(plantType));
        playSound(GetRandomValue(0, 1) ? "PLANT" : "PLANT2", 0.9f);
    }
    return placed;
}

bool World :: trySpawnPlayerZombie(Vector2 position, ZombieType zombieType) {
    if (!playerControlsZombies || !map || !isReady() || !currentLevel) return false;
    if (zombieType < 0 || zombieType >= ZOMBIE_COUNT ||
        zombieType == ZOMBOSS_ZOMBIE || zombieType == ZOMBIE_CHARRED) return false;

    int row, column;
    std :: tie(row, column) = grid.getCellID(position);
    if (row < 0 || column < 0 ||
        row >= static_cast<int>(currentLevel -> getLanes().size()) ||
        currentLevel -> getLanes()[row] == LaneType :: INACTIVE) return false;

    const Rectangle spawnRect = grid.getCellRect(row, 8);
    std :: unique_ptr<Zombie> zombie = zombieFactory.createZombie(
        zombieType,
        Rectangle{
            spawnRect.x + spawnRect.width * 0.5f,
            spawnRect.y - 70.0f,
            50.0f,
            100.0f
        }
    );
    if (!zombie) return false;
    zombieManager.addZombie(std :: move(zombie));
    return true;
}

bool World :: handleParticleClick(Vector2 position) {
    const int collectedValue = particleManager.handleClick(position);
    if(collectedValue <= 0) return false;

    sunAmount += collectedValue;
    playSound("POINTS", 0.8f);
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
    Rectangle rowBounds = grid.getCellRect(lane, 0);
    Rectangle spawnRect = grid.getCellRect(lane, 8);
    zombieManager.addZombie(
        zombieFactory.createZombie(
            type,
            Rectangle{spawnRect.x + 100.0f, rowBounds.y - 40.0f, 50.0f, 100.0f}
        )
    );

    if (GetRandomValue(1, 6) == 1) {
        const char* groans[] = {"GROAN", "GROAN2", "GROAN3", "GROAN4", "GROAN5", "GROAN6", "LOWGROAN"};
        playSound(groans[GetRandomValue(0, 6)], 0.6f);
    }
}

void World::spawnZombieAt(ZombieType type, Vector2 pos) {
    zombieManager.addZombie(
        zombieFactory.createZombie(
            type,
            Rectangle{pos.x, pos.y, 50.0f, 100.0f}
        )
    );
}

void World::playSound(const std::string& key, float volume) {
    if (assetManager && assetManager->getSoundManager()) {
        assetManager->getSoundManager()->play(key, volume);
    }
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

void World :: setPlayerControlsZombies(bool enabled) {
    playerControlsZombies = enabled;
}

int World :: getZombieCountInLane(int lane) const {
    if (lane < 0 || lane >= 5) return 0;

    int count = 0;
    for (const auto& zombie : zombieManager.getZombies()) {
        if (!zombie || zombie -> isDead()) continue;
        const Rectangle hitbox = zombie -> getHitbox();
        const Vector2 center = {
            hitbox.x + hitbox.width * 0.5f,
            hitbox.y + hitbox.height * 0.5f
        };
        if (grid.getCellID(center).first == lane) count++;
    }
    return count;
}

int World :: getZombieCountNearCell(int row, int col, int rowRadius, int colRadius) const {
    if (row < 0 || row >= 5 || col < 0 || col >= 9) return 0;

    int count = 0;
    for (const auto& zombie : zombieManager.getZombies()) {
        if (!zombie || zombie -> isDead()) continue;
        const Rectangle hitbox = zombie -> getHitbox();
        const Vector2 center = {
            hitbox.x + hitbox.width * 0.5f,
            hitbox.y + hitbox.height * 0.5f
        };
        auto [zombieRow, zombieCol] = grid.getCellID(center);
        if (zombieRow < 0) continue;
        if (zombieCol < 0) zombieCol = center.x < grid.getCellRect(zombieRow, 0).x ? 0 : 8;
        if (std::abs(zombieRow - row) <= std::max(0, rowRadius) &&
            std::abs(zombieCol - col) <= std::max(0, colRadius)) count++;
    }
    return count;
}

float World :: getZombieThreatInLane(int lane) const {
    if (lane < 0 || lane >= 5) return 0.0f;

    const Rectangle laneBounds = grid.getCellRect(lane, 0);
    const Rectangle lawnStart = grid.getCellRect(lane, 0);
    const Rectangle lawnEnd = grid.getCellRect(lane, 8);
    const float lawnWidth = lawnEnd.x + lawnEnd.width - lawnStart.x;
    float threat = 0.0f;

    for (const auto& zombie : zombieManager.getZombies()) {
        if (!zombie || zombie -> isDead()) continue;
        const Rectangle hitbox = zombie -> getHitbox();
        const float centerY = hitbox.y + hitbox.height * 0.5f;
        if (centerY < laneBounds.y || centerY > laneBounds.y + laneBounds.height) continue;

        const float proximity = std::clamp(
            (lawnEnd.x + lawnEnd.width - hitbox.x) / std::max(1.0f, lawnWidth),
            0.0f,
            1.5f
        );
        const float durability = (zombie -> getHealth() + zombie -> getArmorHealth()) / 500.0f;
        const float speedFactor = zombie -> getSpeed() / 20.0f;
        threat += 1.0f + durability + speedFactor + proximity * 3.0f;
    }
    return threat;
}

int World :: getNearestZombieColumnInLane(int lane) const {
    if (lane < 0 || lane >= 5) return -1;

    int nearestColumn = 9;
    bool foundZombie = false;
    const Rectangle laneBounds = grid.getCellRect(lane, 0);
    for (const auto& zombie : zombieManager.getZombies()) {
        if (!zombie || zombie -> isDead()) continue;
        const Rectangle hitbox = zombie -> getHitbox();
        const Vector2 center = {
            hitbox.x + hitbox.width * 0.5f,
            hitbox.y + hitbox.height * 0.5f
        };
        if (center.y < laneBounds.y || center.y > laneBounds.y + laneBounds.height) continue;

        int column = grid.getCellID(center).second;
        if (column < 0) column = center.x < laneBounds.x ? 0 : 8;
        nearestColumn = std::min(nearestColumn, column);
        foundZombie = true;
    }
    return foundZombie ? nearestColumn : -1;
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

bool World :: isCurrentWaveHuge() const {
    return waveManager.isCurrentWaveHuge();
}

bool World :: isCurrentWaveFinal() const {
    return waveManager.isCurrentWaveFinal();
}

void World :: updateWorldState() {
    if(wResult != WorldResult :: RUNNING) return;

    if(zombieManager.hasZombieReachedHouse(-45.0f)){
        wResult = playerControlsZombies ? WorldResult :: WON : WorldResult :: LOST;
    }
    else if(!playerControlsZombies && waveManager.hasSpawnAll() and zombieManager.empty()){
        wResult = WorldResult :: WON;
    }
}


void World :: setUserProfileManager(UserProfileManager* user){
    userManager = user;
}
