#include "Gameplay/Particle/ZombiePart.hpp"

ZombiePart :: ZombiePart(
    ZombiePartType partType, Texture2D* texture, Vector2 position,
    Vector2 velocity, float groundY, float angularVelocity,
    float scaleRatio, float lifeTime, Color tint
) : ZombiePart(
        partType,
        std :: vector<ZombiePartLayer>{
            ZombiePartLayer{texture, {0.0f, 0.0f}, 0.0f, 1.0f}
        },
        position,
        velocity,
        groundY,
        angularVelocity,
        scaleRatio,
        lifeTime,
        tint
    ) {}

ZombiePart :: ZombiePart(
    ZombiePartType partType,
    std :: vector<ZombiePartLayer> layers,
    Vector2 position,
    Vector2 velocity,
    float groundY,
    float angularVelocity,
    float scaleRatio,
    float lifeTime,
    Color tint) : Particle(nullptr, position, velocity, {0.0f, 600.0f}, lifeTime, scaleRatio)
{
    this -> partType = partType;
    this -> layers = std :: move(layers);
    this -> groundY = groundY;
    this -> angularVeclocity = angularVelocity;
    this -> tint = tint;
}

void ZombiePart :: update(float dt) {
    if(expired) return;

    Particle :: update(dt);
    if(expired || landed || position.y < groundY) return;

    position.y = groundY;

    if(std :: fabs(velocity.y) > 80.0f){
        velocity.y *= -0.25f;
        velocity.x *= 0.65f;
        angularVeclocity *= 0.65f;
        return;
    }

    landed = true;
    velocity = {0.0f, 0.0f};
    acceleration = {0.0f, 0.0f};
    angularVeclocity = 0.0f;
}

void ZombiePart :: draw() const {
    if(expired || layers.empty()) return;

    const float radians = rotation * DEG2RAD;
    const float cosine = std :: cos(radians);
    const float sine = std :: sin(radians);

    for(const ZombiePartLayer& layer : layers){
        if(layer.texture == nullptr) continue;

        const float localX = layer.offset.x * scaleRatio;
        const float localY = layer.offset.y * scaleRatio;
        const Vector2 layerPosition = {
            position.x + localX * cosine - localY * sine,
            position.y + localX * sine + localY * cosine
        };

        const float layerScale = scaleRatio * layer.scale;
        const float width = static_cast<float>(layer.texture -> width) * layerScale;
        const float height = static_cast<float>(layer.texture -> height) * layerScale;

        DrawTexturePro(
            *layer.texture,
            {
                0.0f,
                0.0f,
                static_cast<float>(layer.texture -> width),
                static_cast<float>(layer.texture -> height)
            },
            {layerPosition.x, layerPosition.y, width, height},
            {width * 0.5f, height * 0.5f},
            rotation + layer.rotation,
            tint
        );
    }
}

ZombiePartType ZombiePart :: getType() const {
    return partType;
}

bool ZombiePart :: isLanded() const {
    return landed;
}
