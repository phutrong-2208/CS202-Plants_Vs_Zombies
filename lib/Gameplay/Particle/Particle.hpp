#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "Common.hpp"

class Particle{
protected:
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};
    Vector2 acceleration = {0.0f, 0.0f};

    float rotation = 0.0f;
    float angularVeclocity = 0.0f;
    float scaleRatio = 1.0f;

    float lifeTime = 2.0f;

    Texture2D* texture = nullptr;
    bool expired = false;
public:
    virtual ~Particle() = default;
    Particle() = default;
    Particle(
        Texture2D* texture,
        Vector2 position,
        Vector2 velocity = {0.0f, 0.0f},
        Vector2 acceleration = {0.0f, 0.0f},
        float lifeTime = 2.0f,
        float scaleRatio = 1.0f
    );
    
    virtual void update(float dt);
    virtual void draw() const;
    virtual int collect(Vector2 mousePosition);

    virtual Rectangle getHitBox() const;
    bool isExpired(void) const;
    void setRotation(float rot) { rotation = rot; }
    void setAngularVelocity(float angVel) { angularVeclocity = angVel; }
};

#endif
