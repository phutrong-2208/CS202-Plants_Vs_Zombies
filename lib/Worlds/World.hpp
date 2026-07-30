#ifndef WORLD_HPP
#define WORLD_HPP

#include <Core/AssetManager.hpp>

#include <Gameplay/IGameplayMediator.hpp>

#include <Worlds/Grid.hpp>
#include <Worlds/Map.hpp>
#include <Worlds/DayMap.hpp>
#include <Gameplay/Plants/PlantFactory.hpp>
#include <Gameplay/Zombies/ZombieFactory.hpp>
#include <Gameplay/Zombies/ZombieManager.hpp>

//Projectiles

#include "Gameplay/Projectile/ProjectileManager.hpp"
#include "Gameplay/Projectile/ProjectileFactory.hpp"
#include "Gameplay/Projectile/Projectile.hpp"


class World : public IGameplayMediator {
private:
    std::unique_ptr <Map> map = nullptr;
    Grid grid;
    PlantFactory plantFactory;
    ZombieFactory zombieFactory;
    ZombieManager zombieManager;
    ProjectileManager projectileManager;
    ProjectileFactory projectileFactory;
    // TexturePackage* projectileTexturePackage = nullptr;
    // static const int NUM_ROWS = 5, NUM_COLS = 9;

    void addProjectile(PlantType plantType, Vector2 position, float damage) override;
    bool hasTarget(PlantType plantType, Vector2 spawnPos, Rectangle bounds) override;
public:
    World() = default;
    ~World() = default;
    World(int screenWidth, int screenHeight, AssetManager* assetManager);

    void update(float dt);
    void draw();
    void drawPlacementPreview(int selectedPlantId) const;
    bool tryPlacePlant(Vector2 position, PlantType plantType);
    bool isReady() const;
    bool isChoosingPlants() const;
    void finishChoosingPlants();

};

#endif
