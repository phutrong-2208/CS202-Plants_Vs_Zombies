#ifndef IGAMEPLAYMEDIATOR_HPP
#define IGAMEPLAYMEDIATOR_HPP

#include <Gameplay/Plants/PlantType.hpp>
#include <Gameplay/Projectile/Projectile.hpp>
#include <Gameplay/Particle/Particle.hpp>
#include <Gameplay/Zombies/ZombieType.hpp>

static const std::map <PlantType, ProjectileType> projectileConvert = {
    {PEASHOOTER, PROJECTILE_PEA},
    {REPEATER,   PROJECTILE_PEA},
    {SNOWPEA,    PROJECTILE_SNOWPEA},
    {CACTUS,     PROJECTILE_CACTUS}
};

class IGameplayMediator {
public:
    virtual ~IGameplayMediator() = default;

    // Plants shooting signals
    virtual void addProjectile(PlantType plantType, Vector2 position, float damage) = 0;
    virtual bool hasTarget(PlantType plantType, Vector2 spawnPos, Rectangle bounds) = 0;

    // Projectile toggling signals
    virtual bool touchTarget(Projectile* projectile) = 0;

    // Zombie killing signals
    virtual bool hasPlantInArea(Rectangle area) const = 0;
    virtual bool damagePlantInArea(Rectangle area, float damage) = 0;

    // Particles
    virtual void addParticle(std::unique_ptr<Particle> particle) = 0;
    virtual void spawnSun(Vector2 position, float targetY, int value) = 0;

    // Wave spawning
    virtual void spawnZombie(ZombieType type, int lane) = 0;
};

#endif
