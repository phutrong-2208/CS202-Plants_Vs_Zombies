#ifndef ZOMBIE_PART_HPP
#define ZOMBIE_PART_HPP

#include "Gameplay/Particle/Particle.hpp"

enum class ZombiePartType{
    HEAD,
    ARM,
    LEG,
    HELMET,
    SHIELD,
    OBJECT
};

struct ZombiePartLayer{
    Texture2D* texture = nullptr;
    Vector2 offset = {0.0f, 0.0f};
    float rotation = 0.0f;
    float scale = 1.0f;
};

class ZombiePart : public Particle{
private:
    ZombiePartType partType = ZombiePartType :: HEAD;
    std :: vector<ZombiePartLayer> layers;
    float groundY = 0.0f;
    bool landed = false;

public:
    ZombiePart(
        ZombiePartType partType,
        Texture2D* texture,
        Vector2 position,
        Vector2 velocity,
        float groundY,
        float angularVelocity = 240.0f,
        float scaleRatio = 1.0f,
        float lifeTime = 3.0f
    );

    ZombiePart(
        ZombiePartType partType,
        std :: vector<ZombiePartLayer> layers,
        Vector2 position,
        Vector2 velocity,
        float groundY,
        float angularVelocity = 240.0f,
        float scaleRatio = 1.0f,
        float lifeTime = 3.0f
    );

    void update(float dt) override;
    void draw() const override;

    ZombiePartType getType() const;
    bool isLanded() const;
};

#endif
