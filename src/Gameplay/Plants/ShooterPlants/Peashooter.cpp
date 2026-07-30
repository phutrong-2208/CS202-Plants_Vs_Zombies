#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"

Peashooter ::Peashooter() = default;

PlantType Peashooter::getType() {
    return PEASHOOTER;
}

Vector2 Peashooter ::getProjectileSpawnPosition() {
    // PeashooterSingle.reanim mouth track, including its 1.5 render scalar.
    return {bounds.x + 108.0f, bounds.y + 45.0f};
}

// bool Peashooter ::hasTarget(const ZombieManager &zombieManager) const {
//   if (!plantData)
//     return false;

//   const Vector2 spawnPosition = getProjectileSpawnPosition();
//   const Rectangle sensor = {spawnPosition.x, bounds.y,
//                             plantData->getProjectileRange(), bounds.height};

//   return zombieManager.hasZombieInArea(sensor);
// }

// void Peashooter ::attack(ProjectileManager &projectileManager,
//                          const ZombieManager &zombieManager) {
//   if (!plantData || !peaTexture)
//     return;
//   if (cooldownTimer > 0.0f)
//     return;
//   if (!hasTarget(zombieManager))
//     return;

//   projectileManager.addProjectile(std ::make_unique<Projectile>(
//       getProjectileSpawnPosition(), Vector2{300.0f, 0.0f},
//       static_cast<int>(plantData->getDamage(false)), 12.0f,
//       plantData->getProjectileRange(), peaTexture));

//   cooldownTimer = plantData->getProjectileCooldown();
// }
