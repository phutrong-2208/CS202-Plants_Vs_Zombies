#include "Gameplay/Zombies/ZombieDeathHandler.hpp"
#include "Gameplay/Particle/ZombiePart.hpp"
#include <memory>

struct ZombieParticleAssets {
    const char* headKey;
    const char* armKey;
};

static ZombieParticleAssets getAssetsForType(ZombieType type) {
    switch (type) {
        case FOOTBALL_ZOMBIE:      return {"ZOMBIEFOOTBALLHEAD", "ZOMBIEARM"};
        case BOBSLED_TEAM_ZOMBIE:  return {"ZOMBIEBOBSLEDHEAD", "ZOMBIEARM"};
        case DANCING_ZOMBIE:       return {"ZOMBIEDANCERHEAD", "ZOMBIEARM"};
        case BACKUP_DANCER_ZOMBIE: return {"ZOMBIEBACKUPDANCERHEAD", "ZOMBIEARM"};
        case DOLPHIN_RIDER_ZOMBIE: return {"ZOMBIEDOLPHINRIDERHEAD", "ZOMBIEARM"};
        case POLE_VAULTING_ZOMBIE: return {"ZOMBIEPOLEVAULTERHEAD", "ZOMBIEARM"};
        case POGO_ZOMBIE:          return {"ZOMBIEPOGOHEAD", "ZOMBIEARM"};
        case LADDER_ZOMBIE:        return {"ZOMBIELADDERHEAD", "ZOMBIEARM"};
        case DIGGER_ZOMBIE:        return {"ZOMBIEDIGGERHEAD", "ZOMBIEDIGGERARM"};
        case IMP_ZOMBIE:           return {"ZOMBIEIMPHEAD", "ZOMBIEARM"};
        case BALLOON_ZOMBIE:       return {"ZOMBIEBALLOONHEAD", "ZOMBIEARM"};
        case YETI_ZOMBIE:          return {"ZOMBIEYETIHEAD", "ZOMBIEARM"};
        case JACK_IN_THE_BOX_ZOMBIE: return {"ZOMBIEHEAD", "ZOMBIEJACKBOXARM"};
        default:                   return {"ZOMBIEHEAD", "ZOMBIEARM"};
    }
}

void ZombieDeathHandler::initialize(TexturePackage* pack, IGameplayMediator* med) {
    particlePack = pack;
    mediator = med;
}

void ZombieDeathHandler::spawnDeathParticles(
    ZombieType type,
    Rectangle hitbox,
    float scalar)
{
    if (mediator == nullptr || particlePack == nullptr) return;

    ZombieParticleAssets assets = getAssetsForType(type);
    
    Texture2D* headTex = particlePack->GetTexture(assets.headKey);
    Texture2D* armTex  = particlePack->GetTexture(assets.armKey);

    const float groundY = hitbox.y + hitbox.height;
    
    // The visual center of the head/arm relative to the hitbox
    Vector2 headPos = { hitbox.x + hitbox.width * 0.5f, hitbox.y - 20.0f };
    Vector2 armPos  = { hitbox.x + hitbox.width * 0.5f, hitbox.y + 30.0f };

    if (headTex) {
        float vx = static_cast<float>(GetRandomValue(-80, 80));
        float vy = static_cast<float>(GetRandomValue(-320, -180));
        float angVel = static_cast<float>(GetRandomValue(-480, 480));
        
        mediator->addParticle(
            std::make_unique<ZombiePart>(
                ZombiePartType::HEAD,
                headTex,
                headPos,
                Vector2{ vx, vy },
                groundY,
                angVel,
                scalar,
                3.0f
            )
        );
    }
    
    if (armTex) {
        float vx = static_cast<float>(GetRandomValue(-120, 120));
        float vy = static_cast<float>(GetRandomValue(-200, -80));
        float angVel = static_cast<float>(GetRandomValue(-600, 600));
        
        mediator->addParticle(
            std::make_unique<ZombiePart>(
                ZombiePartType::ARM,
                armTex,
                armPos,
                Vector2{ vx, vy },
                groundY,
                angVel,
                scalar,
                3.0f
            )
        );
    }
}
