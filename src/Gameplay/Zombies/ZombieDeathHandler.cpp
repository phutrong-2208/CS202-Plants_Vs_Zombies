#include "Gameplay/Zombies/ZombieDeathHandler.hpp"
#include "Gameplay/Particle/ZombiePart.hpp"
#include <memory>
#include <cstring>

// Maps track name prefixes/patterns to a ZombiePartType.
// Order matters: first match wins.
struct TrackTypeRule {
    const char* prefix;
    ZombiePartType partType;
};

static constexpr TrackTypeRule TRACK_TYPE_RULES[] = {
    // Heads / faces (must come before generic 'head' to catch numbered variants)
    { "anim_head",          ZombiePartType::HEAD    },
    { "anim_hair",          ZombiePartType::HEAD    },
    { "anim_jaw",           ZombiePartType::HEAD    },
    { "anim_face",          ZombiePartType::HEAD    },
    { "anim_Zombie_must",   ZombiePartType::HEAD    },

    // Hats / helmets
    { "anim_cone",          ZombiePartType::HELMET  },
    { "anim_bucket",        ZombiePartType::HELMET  },
    { "anim_football",      ZombiePartType::HELMET  },
    { "anim_dancer_hat",    ZombiePartType::HELMET  },
    { "anim_minerhat",      ZombiePartType::HELMET  },

    // Arms / hands
    { "anim_arm",           ZombiePartType::ARM     },
    { "anim_hand",          ZombiePartType::ARM     },

    // Legs / feet
    { "anim_leg",           ZombiePartType::LEG     },
    { "anim_foot",          ZombiePartType::LEG     },

    // Shields / equipment
    { "anim_paper",         ZombiePartType::SHIELD  },
    { "anim_screendoor",    ZombiePartType::SHIELD  },
    { "anim_ladder",        ZombiePartType::SHIELD  },

    // Balloon — treated as an object that flies up
    { "anim_balloon",       ZombiePartType::OBJECT  },

    // Catch-all for body parts / misc objects
    { "anim_body",          ZombiePartType::OBJECT  },
    { "anim_pelvis",        ZombiePartType::OBJECT  },
    { "anim_tie",           ZombiePartType::OBJECT  },
    { "anim_coat",          ZombiePartType::OBJECT  },
    { "anim_sled",          ZombiePartType::OBJECT  },
    { "anim_zamboni",       ZombiePartType::OBJECT  },
    { "anim_pole",          ZombiePartType::OBJECT  },
};

static ZombiePartType classifyTrack(const std::string& name) {
    for (const auto& rule : TRACK_TYPE_RULES) {
        if (name.compare(0, std::strlen(rule.prefix), rule.prefix) == 0) {
            return rule.partType;
        }
    }
    return ZombiePartType::OBJECT; // safe default
}

void ZombieDeathHandler::spawnDeathParticles(
    const ReanimInstance& animation,
    Rectangle hitbox,
    IGameplayMediator* mediator)
{
    if (mediator == nullptr) return;

    // Snapshot every visible track with its actual world-space transform.
    const auto parts = animation.getActiveTrackParts(hitbox);

    for (const TrackSnapshot& snap : parts) {
        // Skip non-body control tracks (they typically have no texture anyway,
        // but guard just in case).
        if (snap.texture == nullptr) continue;
        if (snap.alpha <= 0.0f)      continue;

        const ZombiePartType partType = classifyTrack(snap.trackName);

        // Velocity: heads / helmets fly higher; limbs tumble sideways.
        float vx, vy, angVel;
        switch (partType) {
            case ZombiePartType::HEAD:
            case ZombiePartType::HELMET:
                vx     = static_cast<float>(GetRandomValue(-80, 80));
                vy     = static_cast<float>(GetRandomValue(-320, -180));
                angVel = static_cast<float>(GetRandomValue(-480, 480));
                break;
            case ZombiePartType::ARM:
                vx     = static_cast<float>(GetRandomValue(-120, 120));
                vy     = static_cast<float>(GetRandomValue(-200, -80));
                angVel = static_cast<float>(GetRandomValue(-600, 600));
                break;
            case ZombiePartType::SHIELD:
                vx     = static_cast<float>(GetRandomValue(-150, 150));
                vy     = static_cast<float>(GetRandomValue(-250, -100));
                angVel = static_cast<float>(GetRandomValue(-300, 300));
                break;
            default:
                vx     = static_cast<float>(GetRandomValue(-60, 60));
                vy     = static_cast<float>(GetRandomValue(-160, -40));
                angVel = static_cast<float>(GetRandomValue(-240, 240));
                break;
        }

        // Parts spawn at their actual screen position (world-space from snapshot).
        const Vector2 spawnPos = { snap.worldX, snap.worldY };
        const float   groundY  = hitbox.y + hitbox.height;

        // Scale: keep the original reanim scalar already baked into snap.scaleX.
        const float scale = (snap.scaleX + snap.scaleY) * 0.5f;

        mediator->addParticle(
            std::make_unique<ZombiePart>(
                partType,
                snap.texture,
                spawnPos,
                Vector2{ vx, vy },
                groundY,
                angVel,
                (scale > 0.0f ? scale : 1.0f),
                3.0f            // lifetime seconds
            )
        );
    }
}
