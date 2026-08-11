#include "Gameplay/Zombies/ZombieDeathHandler.hpp"
#include "Gameplay/Particle/ZombiePart.hpp"
#include <memory>
#include <vector>

struct TrackPartMapping {
    const char* trackName;
    ZombiePartType partType;
};

static constexpr TrackPartMapping ZOMBIE_TRACKS[] = {
    // Heads and Faces
    {"anim_head1", ZombiePartType::HEAD},
    {"anim_head2", ZombiePartType::HEAD},
    {"anim_head3", ZombiePartType::HEAD}, 
    {"anim_hair", ZombiePartType::HEAD},
    {"anim_jaw", ZombiePartType::HEAD},
    {"anim_Zombie_mustache", ZombiePartType::HEAD}, // Mustaches

    // Hats and Helmets
    {"anim_cone", ZombiePartType::HELMET},
    {"anim_bucket", ZombiePartType::HELMET},
    {"anim_footballhelmet", ZombiePartType::HELMET},
    {"anim_dancer_hat", ZombiePartType::HELMET}, // Assuming similar names if they exist
    {"anim_minerhat", ZombiePartType::HELMET},
    {"anim_balloon", ZombiePartType::OBJECT}, // Balloon Zombie's balloon

    // Arms and Hands
    {"anim_arm_upper_left", ZombiePartType::ARM},
    {"anim_arm_lower_left", ZombiePartType::ARM},
    {"anim_hand_left", ZombiePartType::ARM},
    {"anim_arm_upper_right", ZombiePartType::ARM},
    {"anim_arm_lower_right", ZombiePartType::ARM},
    {"anim_hand_right", ZombiePartType::ARM},
    
    // Legs and Feet
    {"anim_leg_upper_left", ZombiePartType::LEG},
    {"anim_leg_lower_left", ZombiePartType::LEG},
    {"anim_foot_left", ZombiePartType::LEG},
    {"anim_leg_upper_right", ZombiePartType::LEG},
    {"anim_leg_lower_right", ZombiePartType::LEG},
    {"anim_foot_right", ZombiePartType::LEG},
    
    // Body and Clothes
    {"anim_body1", ZombiePartType::OBJECT},
    {"anim_body2", ZombiePartType::OBJECT},
    {"anim_pelvis", ZombiePartType::OBJECT},
    {"anim_tie", ZombiePartType::OBJECT},
    {"anim_coat", ZombiePartType::OBJECT},
    
    // Shields and Equipment
    {"anim_paper", ZombiePartType::SHIELD},
    {"anim_screendoor", ZombiePartType::SHIELD},
    {"anim_ladder", ZombiePartType::SHIELD},
    {"anim_sled", ZombiePartType::OBJECT}, // Bobsled
    {"anim_zamboni", ZombiePartType::OBJECT},
    {"anim_pole", ZombiePartType::OBJECT} // Pole vault
};

void ZombieDeathHandler::spawnDeathParticles(const ReanimInstance& animation, Rectangle hitbox, IGameplayMediator* mediator) {
    if (mediator == nullptr) return;

    for (const auto& mapping : ZOMBIE_TRACKS) {
        Texture2D* tex = animation.getTrackTexture(mapping.trackName);
        if (tex != nullptr) {
            // Give parts a random velocity to look like an explosion
            float vx = GetRandomValue(-100, 100) / 1.0f;
            float vy = GetRandomValue(-250, -50) / 1.0f;
            float angVel = GetRandomValue(-360, 360) / 1.0f;
            
            // Adjust starting position roughly around the center of the hitbox
            Vector2 spawnPos = {hitbox.x + hitbox.width * 0.5f, hitbox.y + hitbox.height * 0.5f};
            
            mediator->addParticle(
                std::make_unique<ZombiePart>(
                    mapping.partType,
                    tex,
                    spawnPos,
                    Vector2{vx, vy},
                    hitbox.y + hitbox.height, // Ground Y
                    angVel,
                    1.25f // Scale
                )
            );
        }
    }
}
