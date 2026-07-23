#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Gameplay/Plants/Plant.hpp"

class Peashooter : public Plant {
private:
    Texture2D* peaTexture = nullptr;

public:
    Peashooter();
    void setProjectileTexture(Texture2D* texture);
    Vector2 getProjectileSpawnPosition() const;
    bool hasTarget(const ZombieManager& zombieManager) const override;
    void attack(
        ProjectileManager& projectileManager,
        const ZombieManager& zombieManager
    ) override;
};

#endif
