#ifndef IGAMEPLAYMEDIATOR_HPP
#define IGAMEPLAYMEDIATOR_HPP

#include <Gameplay/Plants/PlantType.hpp>
#include <Gameplay/Projectile/Projectile.hpp>
#include <Gameplay/Particle/Particle.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>

class Zombie;

static const std::map <PlantType, ProjectileType> projectileConvert = {
    {PEASHOOTER, PROJECTILE_PEA},
    {REPEATER,   PROJECTILE_PEA},
    {SNOWPEA,    PROJECTILE_SNOWPEA},
    {CACTUS,     PROJECTILE_CACTUS},
    {THREEPEATER, PROJECTILE_PEA},
    {GATLINGPEA, PROJECTILE_PEA},
    {SPLITPEA,   PROJECTILE_PEA},
    {STARFRUIT,  PROJECTILE_STAR},
    {PUFFSHROOM, PROJECTILE_PUFF},
    {FUMESHROOM, PROJECTILE_PUFF},
    {SCAREDYSHROOM, PROJECTILE_PUFF},
    {GLOOMSHROOM, PROJECTILE_PUFF},
    {SEASHROOM,  PROJECTILE_PUFF},
    {CABBAGEPULT, PROJECTILE_CABBAGE},
    {CORNPULT,    PROJECTILE_CORN},
    {MELONPULT,   PROJECTILE_MELON},
    {WINTERMELON, PROJECTILE_WINTERMELON},
    {COBCANNON,   PROJECTILE_MELON},
    {CATTAIL,     PROJECTILE_CACTUS}
};

class IGameplayMediator {
public:
    virtual ~IGameplayMediator() = default;

    // Plants shooting signals
    virtual void addProjectile(PlantType plantType, Vector2 position, float damage, bool reverse = false) = 0;
    virtual bool hasTarget(PlantType plantType, Vector2 spawnPos, Rectangle bounds) = 0;

    // Projectile toggling signals
    virtual bool touchTarget(Projectile* projectile) = 0;

    // Zombie killing signals
    virtual bool hasPlantInArea(Rectangle area) const = 0;
    virtual bool damagePlantInArea(Rectangle area, float damage) = 0;
    virtual bool hasZombieInArea(Rectangle area) const = 0;
    virtual void damageZombiesInArea(Rectangle area, float damage) = 0;
    virtual Zombie* getZombiePriority(Rectangle area) = 0;
    virtual void collectSuns() = 0;
    virtual void freezeZombiesInArea(Rectangle area, float duration) = 0;

    // Particles
    virtual void addParticle(std::unique_ptr<Particle> particle) = 0;
    virtual void spawnExplosionParticles(Vector2 position, PlantType type) = 0;
    virtual void spawnSun(Vector2 position, float targetY, int value) = 0;

    // Wave spawning
    virtual void spawnZombie(ZombieType type, int lane) = 0;
};

#endif
