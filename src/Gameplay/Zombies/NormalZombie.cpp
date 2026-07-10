#include "Gameplay/Zombies/NormalZombie.hpp"

namespace { // STATS
    constexpr int NORMAL_ZOMBIE_HEALTH = 270;
    constexpr float NORMAL_ZOMBIE_SPEED = 18.0f;
    constexpr int NORMAL_ZOMBIE_DAMAGE = 100;
}

NormalZombie :: NormalZombie(Rectangle hitbox)
    : Zombie(hitbox, NORMAL_ZOMBIE_HEALTH, NORMAL_ZOMBIE_SPEED, NORMAL_ZOMBIE_DAMAGE)
{}
