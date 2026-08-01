#include "Gameplay/Particle/Particle.hpp"

Particle :: Particle(
    Texture2D* texture,
    Vector2 position,
    Vector2 velocity,
    Vector2 acceleration,
    float lifeTime,
    float scaleRatio
) :
    position(position),
    velocity(velocity),
    acceleration(acceleration),
    scaleRatio(scaleRatio),
    lifeTime(lifeTime),
    texture(texture) {}


void Particle :: update(float dt){
    if(expired) return;

    velocity.x += acceleration.x * dt;
    velocity.y += acceleration.y * dt;

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    rotation += angularVeclocity * dt;

    lifeTime -= dt;
    if(lifeTime <= 0){
        expired = true;
    }
}

void Particle :: draw() const {
    if(expired || texture == nullptr) return;

    const float width = static_cast<float>(texture -> width) * scaleRatio;
    const float height = static_cast<float>(texture -> height) * scaleRatio;
    const Rectangle src = {
        0.0f, 0.0f,
        static_cast<float>(texture -> width),
        static_cast<float>(texture -> height)
    };
    const Rectangle dst = {
        position.x, position.y, width, height
    };

    DrawTexturePro(
        *texture, src, dst,
        {width * 0.5f, height * 0.5f},
        rotation, WHITE
    );
}

int Particle :: collect(Vector2 mousePosition) {
    return 0;
}

Rectangle Particle :: getHitBox() const {
    if(texture == nullptr) {
        return {position.x, position.y, 0.0f, 0.0f};
    }

    const float width = static_cast<float>(texture -> width) * scaleRatio;
    const float height = static_cast<float>(texture -> height) * scaleRatio;
    return {
        position.x - width * 0.5f, position.y - height * 0.5f,
        width, height
    };
}

bool Particle :: isExpired(void) const{
    return expired;
}
