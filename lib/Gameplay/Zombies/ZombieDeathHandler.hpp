#ifndef ZOMBIEDEATHHANDLER_HPP
#define ZOMBIEDEATHHANDLER_HPP

#include "Gameplay/Animation/ReanimInstance.hpp"
#include "Gameplay/IGameplayMediator.hpp"
#include <Core/TexturePackage.hpp>
#include <raylib.h>

class ZombieDeathHandler {
private:
    TexturePackage* particlePack = nullptr;
    IGameplayMediator* mediator = nullptr;
public:
    ZombieDeathHandler() = default;
    void initialize(TexturePackage* pack, IGameplayMediator* med);
    void spawnDeathParticles(ZombieType type, Rectangle hitbox, float scalar, Color tint = WHITE);
};

#endif
