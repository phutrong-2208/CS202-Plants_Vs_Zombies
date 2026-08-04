#ifndef WORLD_HPP
#define WORLD_HPP

#include <Core/AssetManager.hpp>

#include <Gameplay/IGameplayMediator.hpp>

#include <Worlds/Grid.hpp>
#include <Worlds/Map.hpp>
#include <Worlds/DayMap.hpp>
#include <Gameplay/Plants/PlantFactory.hpp>


// Zombies
#include <Gameplay/Zombies/ZombieFactory.hpp>
#include <Gameplay/Zombies/ZombieManager.hpp>

// Projectiles

#include "Gameplay/Projectile/ProjectileManager.hpp"
#include "Gameplay/Projectile/ProjectileFactory.hpp"
#include "Gameplay/Projectile/Projectile.hpp"

// Particles
#include <Gameplay/Particle/ParticleManager.hpp>
#include <Gameplay/Particle/Sun.hpp>


class World : public IGameplayMediator {
private:
    std::unique_ptr <Map> map = nullptr;
    Grid grid;
    PlantFactory plantFactory;
    ZombieFactory zombieFactory;
    ZombieManager zombieManager;
    ProjectileManager projectileManager;
    ProjectileFactory projectileFactory;
    ParticleManager particleManager;
    
    int sunAmount = 50;

    // Sky Sun properties for textures
    TexturePackage* sunPackage = nullptr;
    ReanimParser* sunAnimationData = nullptr;

    void addProjectile(PlantType plantType, Vector2 position, float damage) override;
    bool hasTarget(PlantType plantType, Vector2 spawnPos, Rectangle bounds) override;
    bool touchTarget(Projectile* projectile) override;
    bool hasPlantInArea(Rectangle area) const override;
    bool damagePlantInArea(Rectangle area, float damage) override;
    void addParticle(std::unique_ptr<Particle> particle) override;
    void spawnSun(Vector2 position, float targetY) override;
public:
    World() = default;
    ~World() = default;
    World(int screenWidth, int screenHeight, AssetManager* assetManager);

    void update(float dt);
    void draw();
    void drawPlacementPreview(int selectedPlantId) const;
    bool tryPlacePlant(Vector2 position, PlantType plantType);
    bool handleParticleClick(Vector2 position);
    int getSunAmount() const;
    bool isReady() const;
    bool isChoosingPlants() const;
    void finishChoosingPlants();
};

#endif
