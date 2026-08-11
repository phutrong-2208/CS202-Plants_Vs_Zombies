#ifndef ZOMBIEDEATHHANDLER_HPP
#define ZOMBIEDEATHHANDLER_HPP

#include "Gameplay/Animation/ReanimInstance.hpp"
#include "Gameplay/IGameplayMediator.hpp"
#include <raylib.h>

class ZombieDeathHandler {
public:
    static void spawnDeathParticles(const ReanimInstance& animation, Rectangle hitbox, IGameplayMediator* mediator);
};

#endif
